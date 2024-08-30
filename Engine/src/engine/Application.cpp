#include "Application.h"

#include "ecs/systems/RootTransformSystem.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/WorldTransformSystem.h"
#include "input/InputComponent.h"
#include "input/InputSystem.h"
#include "platform/PlatformRunLoop.h"
#include "platform/IWindow.h"
#include "render/Renderer.h"
#include "io/VFS.h"
#include "render/systems/MeshRenderSystem.h"
#include "render/systems/PointLightSystem.h"
#include "ui/observers/ButtonObserver.h"
#include "ui/observers/RectTransformObserver.h"
#include "ui/observers/TitleBarObserver.h"
#include "ui/observers/WindowObserver.h"
#include "ui/systems/ImageRenderSystem.h"
#include "ui/systems/RectTransformSystem.h"
#include "ui/systems/RootRectTransformSystem.h"
#include "ui/systems/TextRenderSystem.h"
#include "ui/systems/ButtonSystem.h"
#include "ui/systems/TitleBarSystem.h"
#include "ui/systems/UIKeyboardInputSystem.h"
#include "ui/systems/UIMouseInputSystem.h"

namespace se
{
    Application* Application::s_Instance = nullptr;

    Application* Application::Get()
    {
        return s_Instance;
    }

    void Application::Init()
    {
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
    }

    void Application::CreateInitialObservers()
    {
        m_World.CreateObserver<ui::observers::ButtonObserver, ui::components::ButtonComponent>();
        m_World.CreateObserver<ui::observers::RectTransformObserver, ui::components::RectTransformComponent>();
        m_World.CreateObserver<ui::observers::WindowObserver, ui::components::WindowComponent>();
        m_World.CreateObserver<ui::observers::TitleBarObserver, ui::components::TitleBarComponent>();
    }

    void Application::CreateInitialSystems()
    {
        auto input = m_World.CreateEngineSystem<input::InputSystem>({}, {}, {});
        m_World.CreateEngineSystem<ui::systems::UIMouseInputSystem>({}, {}, { input });
        m_World.CreateEngineSystem<ui::systems::UIKeyboardInputSystem>({}, {}, { input });
        m_World.CreateEngineSystem<render::systems::PointLightSystem>({}, {}, {});
        m_World.CreateEngineSystem<render::systems::MeshRenderSystem>({}, {}, {});
        auto rootTransform = m_World.CreateEngineSystem<ecs::systems::RootTransformSystem>({}, {}, {});
        auto worldTransform = m_World.CreateEngineSystem<ecs::systems::WorldTransformSystem>({}, {}, { rootTransform });
        m_World.CreateEngineSystem<ecs::systems::TransformSystem>({}, {}, { worldTransform });
        auto rootRect = m_World.CreateEngineSystem<ui::systems::RootRectTransformSystem>({}, {}, {});
        m_World.CreateEngineSystem<ui::systems::RectTransformSystem>({}, {}, { rootRect });
        m_World.CreateEngineSystem<ui::systems::ButtonSystem>({}, {}, {});
        m_World.CreateEngineSystem<ui::systems::TitleBarSystem>({}, {}, {});
        m_World.CreateEngineSystem<ui::systems::ImageRenderSystem>({}, {}, { });
        m_World.CreateEngineSystem<ui::systems::TextRenderSystem>({}, {}, {});
    }

    void Application::Run() const
    {
        while (!m_RunLoop->ShouldExit())
        {
            m_RunLoop->Update();
        }
    }

    void Application::Shutdown()
    {
        delete m_RunLoop;
        m_RunLoop = nullptr;
    }

    void Application::Update()
    {
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
        m_World.Render();
    }
}
