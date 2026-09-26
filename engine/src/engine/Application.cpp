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
#include "Components.generated.h"
#include "ui/observers/EditableTextObserver.h"

#include "Widgets.generated.h"
#include "Classes.generated.h"
#include "Systems.generated.h"
#include "camera/ActiveCameraComponent.h"
#include "render/components/PointLightComponent.h"
#include "render/singleton_components/MeshRenderComponent.h"
#include "../../../editor/src/editor/singleton_components/DragDropStateComponent.h"
#include "editor/singleton_components/EditorPickComponent.h"
#include "geo/singleton_components/CollisionComponent.h"
#include "platform/MouseCursorUtil.h"
#include "threads/util/ThreadUtil.h"

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
        threads::util::SetMainThread(std::this_thread::get_id());

#if SPARK_EDITOR
        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "engine_assets"), "/engine_source_assets", false);
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "assets"), "/source_assets", false);
#endif
        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "built"), "/engine_assets", true);
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "built"), "/assets", true);
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "config"), "/config", true);
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "save"), "/save", false);
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "temp"), "/tmp", false);

        LoadPrefs();

        render::Renderer::Create();
        m_PrimaryWindow = IWindow::CreatePlatformWindow(m_WindowPosX, m_WindowPosY, m_WindowSizeX, m_WindowSizeY);
        m_PrimaryWindow->OnResized.Subscribe([this](int x, int y)
        {
            m_WindowSizeX = x;
            m_WindowSizeY = y;
            SavePrefs();
        });
        m_PrimaryWindow->OnMoved.Subscribe([this](int x, int y)
        {
            m_WindowPosX = x;
            m_WindowPosY = y;
            SavePrefs();
        });
        m_TimeLastFrame = std::chrono::system_clock::now();

        MouseCursorUtil::InitCursors();

        RegisterComponents(&m_World);
        engine_InitClassReflection();

        m_World.CreateScene("Default");

        CreateInitialSingletonComponents();
        engine_InitSystems(&m_World);

#if SPARK_EDITOR
        m_Editor.Init();
#endif

        CreateInitialObservers();

        m_World.Init();
    }

    void Application::CreateInitialSingletonComponents()
    {
        m_World.AddSingletonComponent<input::InputComponent>();
        m_World.AddSingletonComponent<camera::ActiveCameraComponent>();
        m_World.AddSingletonComponent<ui::singleton_components::UIRenderComponent>();
        m_World.AddSingletonComponent<render::singleton_components::MeshRenderComponent>();
        m_World.AddSingletonComponent<geo::singleton_components::CollisionComponent>();

#if SPARK_EDITOR
        m_World.AddSingletonComponent<editor::singleton_components::DragDropStateComponent>();
        m_World.AddSingletonComponent<editor::singleton_components::EditorShortcutsComponent>();
        m_World.AddSingletonComponent<editor::singleton_components::EditorPickComponent>();
#endif
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
        MouseCursorUtil::FreeCursors();
    }

    math::IntVec2 Application::GetGameViewportSize()
    {
#if SPARK_EDITOR
        return m_Editor.GetFrameBuffer()->GetSize() * m_PrimaryWindow->GetContentScale();
#else
        return math::IntVec2(m_PrimaryWindow->GetWidth(), m_PrimaryWindow->GetHeight());
#endif
    }

    void Application::Update()
    {
        EASY_BLOCK("Application::Update");
        const auto now = std::chrono::system_clock::now();
        const std::chrono::duration<float> elapsed_seconds = now - m_TimeLastFrame;
        m_DeltaTime = elapsed_seconds.count();
        m_TimeLastFrame = now;

        m_World.Update();

#if SPARK_EDITOR
        m_Editor.Update();
        m_World.ProcessAllPending();
#endif
    }

    void Application::Render()
    {
        EASY_BLOCK("Application::Render");

#if SPARK_EDITOR
        m_Editor.Render();
#endif

        m_World.Render();
    }

    void Application::LoadPrefs()
    {
        if (!io::VFS::Get().Exists("/save/app_prefs.json"))
        {
            return;
        }

        nlohmann::json json = nlohmann::json::parse(io::VFS::Get().ReadText("/save/app_prefs.json"));
        m_WindowSizeX = json["WindowSizeX"];
        m_WindowSizeY = json["WindowSizeY"];
        m_WindowPosX = json["WindowPosX"];
        m_WindowPosY = json["WindowPosY"];
        io::VFS::Get().WriteText("/save/app_prefs.json", json.dump(4));
    }

    void Application::SavePrefs()
    {
        nlohmann::json json;
        json["WindowSizeX"] = m_WindowSizeX;
        json["WindowSizeY"] = m_WindowSizeY;
        json["WindowPosX"] = m_WindowPosX;
        json["WindowPosY"] = m_WindowPosY;
        io::VFS::Get().WriteText("/save/app_prefs.json", json.dump(4));
    }
}
