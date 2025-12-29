#include "Application.h"
#include "engine/ui/observers/ScrollBoxObserver.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "input/InputComponent.h"
#include "platform/IWindow.h"
#include "render/Renderer.h"
#include "io/VFS.h"
#include "ui/components/TreeNodeComponent.h"
#include "ui/observers/ButtonObserver.h"
#include "ui/observers/RectTransformObserver.h"
#include "ui/observers/TitleBarObserver.h"
#include "ui/systems/TextRenderSystem.h"
#include "ui/systems/ButtonSystem.h"
#include "engine/ui/observers/TreeNodeObserver.h"
#include "engine/ui/observers/ImageObserver.h"
#include "engine/ui/observers/TextObserver.h"
#include "ComponentRegistration.generated.h"
#include "ui/observers/EditableTextObserver.h"

#include "Widgets.generated.h"
#include "Classes.generated.h"
#include "Systems.generated.h"
#include "camera/ActiveCameraComponent.h"
#include "render/components/PointLightComponent.h"

namespace se
{
    Application* Application::s_Instance = nullptr;

    // ReSharper disable once CppDFAConstantFunctionResult
    Application* Application::Get()
    {
        return s_Instance;
    }

    void Application::Init()
    {
        render::Renderer::Create();
        m_PrimaryWindow = IWindow::CreatePlatformWindow(1280, 720);
        m_TimeLastFrame = std::chrono::system_clock::now();

        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "built"), "/engine_assets");
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "built"), "/assets");

        RegisterComponents(&m_World);
        engine_InitClassReflection();

#if SPARK_EDITOR
        m_EditorRuntime.Init();
#endif

        CreateInitialSingletonComponents();
        CreateInitialObservers();
        engine_InitSystems(&m_World);
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
        m_World.CreateObserver<ui::observers::EditableTextObserver, ui::components::EditableTextComponent>();
        m_World.CreateObserver<ui::observers::ScrollBoxObserver, ui::components::ScrollBoxComponent>();
    }

    void Application::Shutdown()
    {
    }

    math::IntVec2 Application::GetGameViewportSize()
    {
#if SPARK_EDITOR
        return m_EditorRuntime.GetFrameBuffer()->GetSize() * m_PrimaryWindow->GetContentScale();
#else
        return math::IntVec2(m_PrimaryWindow->GetWidth(), m_PrimaryWindow->GetHeight());
#endif
    }

    void Application::Update()
    {
        EASY_BLOCK("Application::Update");
        const auto now = std::chrono::system_clock::now();
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
        EASY_BLOCK("Application::Render");

#if SPARK_EDITOR
        m_EditorRuntime.Render();
#endif

        m_World.Render();
    }
}
