#include <engine/ui/systems/ScrollBoxUpdateSystem.h>
#include "Application.h"
#include "engine/ui/systems/UIRenderSystem.h"
#include "engine/ui/systems/ScrollBoxRenderSystem.h"
#include "engine/ui/observers/ScrollBoxObserver.h"
#include "engine/ui/systems/WidgetVisibilitySystem.h"
#include "engine/ui/systems/ResetInputSystem.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "ecs/systems/RootTransformSystem.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/WorldTransformSystem.h"
#include "input/InputComponent.h"
#include "input/InputSystem.h"
#include "platform/PlatformRunLoop.h"
#include "platform/IWindow.h"
#include "render/Renderer.h"
#include "io/VFS.h"
#include "profiling/Profiler.h"
#include "render/systems/MeshRenderSystem.h"
#include "render/systems/PointLightSystem.h"
#include "ui/components/TreeNodeComponent.h"
#include "ui/observers/ButtonObserver.h"
#include "ui/observers/RectTransformObserver.h"
#include "ui/observers/TitleBarObserver.h"
#include "ui/systems/ImageRenderSystem.h"
#include "ui/systems/RectTransformSystem.h"
#include "ui/systems/RootRectTransformSystem.h"
#include "ui/systems/TextRenderSystem.h"
#include "ui/systems/ButtonSystem.h"
#include "ui/systems/TitleBarSystem.h"
#include "ui/systems/TreeViewSystem.h"
#include "ui/systems/UIKeyboardInputSystem.h"
#include "ui/systems/UIMouseInputSystem.h"
#include "engine/ui/systems/TreeNodeSystem.h"
#include "engine/ui/observers/TreeNodeObserver.h"
#include "engine/ui/observers/ImageObserver.h"
#include "engine/ui/observers/TextObserver.h"

namespace se
{
    Application* Application::s_Instance = nullptr;

    Application* Application::Get()
    {
        return s_Instance;
    }

    void Application::Init()
    {
        PROFILE_BEGIN_FRAME()
        PROFILE_BEGIN_THREAD()

        m_RunLoop = PlatformRunLoop::CreatePlatformRunloop({});
        render::Renderer::Create();
        m_PrimaryWindow = IWindow::CreatePlatformWindow(1280, 720);
        m_PrimaryWindow->SetCurrent();
        m_TimeLastFrame = std::chrono::system_clock::now();

        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "builtin_assets"), "/builtin_source_assets");
        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "built"), "/builtin_assets");
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "assets"), "/source_assets");
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "built"), "/assets");

#if SPARK_EDITOR
        m_EditorRuntime.Init();
#endif

        CreateInitialSingletonComponents();
        CreateInitialObservers();
        CreateInitialSystems();
        m_World.Init();
    }

    void Application::CreateInitialSingletonComponents()
    {
        m_World.AddSingletonComponent<input::InputComponent>();
        m_World.AddSingletonComponent<camera::ActiveCameraComponent>();
        m_World.AddSingletonComponent<ui::singleton_components::UIRenderComponent>();
    }

    void Application::CreateInitialObservers()
    {
        m_World.CreateObserver<ui::observers::ButtonObserver, ui::components::ButtonComponent>();
        m_World.CreateObserver<ui::observers::RectTransformObserver, ui::components::RectTransformComponent>();
        m_World.CreateObserver<ui::observers::TitleBarObserver, ui::components::TitleBarComponent>();
        m_World.CreateObserver<ui::observers::TreeNodeObserver, ui::components::TreeNodeComponent>();
        m_World.CreateObserver<ui::observers::ImageObserver, ui::components::ImageComponent>();
        m_World.CreateObserver<ui::observers::TextObserver, ui::components::TextComponent>();
        m_World.CreateObserver<ui::observers::ScrollBoxObserver, ui::components::ScrollBoxComponent>();
    }

    void Application::CreateInitialSystems()
    {
        auto input = m_World.CreateEngineSystem<input::InputSystem>("InputSystem", {}, {}, {});
        auto resetInput = m_World.CreateEngineSystem<ui::systems::ResetInputSystem>("ResetInputSystem", {}, {}, { input });
        auto mouseInput = m_World.CreateEngineSystem<ui::systems::UIMouseInputSystem>("UIMouseInputSystem", {}, {}, { resetInput });
        m_World.CreateEngineSystem<ui::systems::UIKeyboardInputSystem>("UIKeyboardInputSystem", {}, {}, { resetInput });
        m_World.CreateEngineSystem<render::systems::PointLightSystem>("PointLightSystem", {}, {}, {});
        m_World.CreateEngineSystem<render::systems::MeshRenderSystem>("MeshRenderSystem", {}, {}, {});
        auto rootTransform = m_World.CreateEngineSystem<ecs::systems::RootTransformSystem>("RootTransformSystem", {}, {}, {});
        auto worldTransform = m_World.CreateEngineSystem<ecs::systems::WorldTransformSystem>("WorldTransformSystem", {}, {}, { rootTransform });
        m_World.CreateEngineSystem<ecs::systems::TransformSystem>("TransformSystem", {}, {}, { worldTransform });
        auto rootRect = m_World.CreateEngineSystem<ui::systems::RootRectTransformSystem>("RootRectTransformSystem", {}, {}, {});

        auto treeNodes = m_World.CreateEngineSystem<ui::systems::TreeNodeSystem>("TreeNodeSystem", {}, {}, { mouseInput });
        m_World.RegisterComponent<ui::components::TreeNodeComponent>();
        m_World.RegisterComponent<ui::components::WidgetComponent>();
        auto treeViewChildQuery =
        {
            std::make_pair<ecs::Id, ecs::ComponentMutability::Type>(ui::components::TreeNodeComponent::GetComponentId(), ecs::ComponentMutability::Immutable),
            std::make_pair<ecs::Id, ecs::ComponentMutability::Type>(ui::components::RectTransformComponent::GetComponentId(), ecs::ComponentMutability::Mutable),
            std::make_pair<ecs::Id, ecs::ComponentMutability::Type>(ui::components::WidgetComponent::GetComponentId(), ecs::ComponentMutability::Mutable)
        };
        auto treeView = m_World.CreateEngineSystem<ui::systems::TreeViewSystem>("TreeViewSystem", {}, treeViewChildQuery, { rootRect, treeNodes });
        m_World.CreateEngineSystem<ui::systems::RectTransformSystem>("RectTransformSystem", {}, {}, { rootRect, treeView });
        m_World.CreateEngineSystem<ui::systems::ButtonSystem>("ButtonSystem", {}, {}, { mouseInput });
        m_World.CreateEngineSystem<ui::systems::TitleBarSystem>("TitleBarSystem", {}, {}, { mouseInput });
        auto scrollBoxChildQuery =
        {
            std::make_pair<ecs::Id, ecs::ComponentMutability::Type>(ui::components::WidgetComponent::GetComponentId(), ecs::ComponentMutability::Mutable),
        };
        m_World.CreateEngineSystem<ui::systems::ScrollBoxUpdateSystem>("ScrollBoxUpdateSystem", {}, scrollBoxChildQuery, { mouseInput });
        auto imageRender = m_World.CreateEngineSystem<ui::systems::ImageRenderSystem>("ImageRenderSystem", {}, {}, { });
        auto textRender = m_World.CreateEngineSystem<ui::systems::TextRenderSystem>("TextRenderSystem", {}, {}, {});
        auto scrollBoxRender = m_World.CreateEngineSystem<ui::systems::ScrollBoxRenderSystem>("ScrollBoxRenderSystem", {}, {}, {});
        m_World.CreateEngineSystem<ui::systems::WidgetVisibilitySystem>("WidgetVisibilitySystem", {}, {}, {});
        m_World.CreateEngineSystem<ui::systems::UIRenderSystem>("UIRenderSystem", {}, {}, { imageRender, textRender, scrollBoxRender });
    }

    void Application::Run() const
    {
        while (!m_RunLoop->ShouldExit())
        {
            m_RunLoop->Update();
            PROFILE_BEGIN_FRAME()
            PROFILE_BEGIN_THREAD()
        }
    }

    void Application::Shutdown()
    {
        delete m_RunLoop;
        m_RunLoop = nullptr;
    }

    void Application::Update()
    {
        PROFILE_SCOPE("Application::Update")
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed_seconds = now - m_TimeLastFrame;
        m_DeltaTime = elapsed_seconds.count();
        m_TimeLastFrame = now;

        m_World.Update();

#if SPARK_EDITOR
        m_EditorRuntime.Update();
#endif
    }

    void Application::Render()
    {
        PROFILE_SCOPE("Application::Render")
        m_World.Render();
    }
}
