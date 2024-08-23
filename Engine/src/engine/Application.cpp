#include "Application.h"

#include "ecs/systems/RootTransformSystem.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/WorldTransformSystem.h"
#include "input/InputComponent.h"
#include "platform/PlatformRunLoop.h"
#include "platform/IWindow.h"
#include "render/Renderer.h"
#include "io/VFS.h"
#include "render/systems/MeshRenderSystem.h"
#include "render/systems/PointLightSystem.h"
#include "ui/systems/ImageRenderSystem.h"
#include "ui/systems/RectTransformSystem.h"
#include "ui/systems/RootRectTransformSystem.h"
#include "ui/systems/TextRenderSystem.h"

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
        CreateInitialSystems();
        m_World.Init();
    }

    void Application::CreateInitialSingletonComponents()
    {
        m_World.AddSingletonComponent<input::InputComponent>();
    }

    void Application::CreateInitialSystems()
    {
        m_World.CreateEngineSystem<render::systems::PointLightSystem>({}, {});
        m_World.CreateEngineSystem<render::systems::MeshRenderSystem>({}, {});
        m_World.CreateEngineSystem<ecs::systems::TransformSystem>({}, {});
        m_World.CreateEngineSystem<ecs::systems::WorldTransformSystem>({}, {});
        m_World.CreateEngineSystem<ecs::systems::RootTransformSystem>({}, {});
        m_World.CreateEngineSystem<ui::systems::RectTransformSystem>({}, {});
        m_World.CreateEngineSystem<ui::systems::RootRectTransformSystem>({}, {});
        m_World.CreateEngineSystem<ui::systems::ImageRenderSystem>({}, {});
        m_World.CreateEngineSystem<ui::systems::TextRenderSystem>({}, {});
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
