#include "Application.h"

#include "platform/PlatformRunLoop.h"
#include "platform/IWindow.h"
#include "render/Renderer.h"
#include "io/VFS.h"

namespace se
{
    Application* Application::s_Instance = nullptr;

    Application* Application::Get()
    {
        return s_Instance;
    }

    void Application::Init()
    {
        render::Renderer::Create();
        m_PrimaryWindow = IWindow::CreatePlatformWindow(1280, 720);
        m_RunLoop = PlatformRunLoop::CreatePlatformRunloop({m_PrimaryWindow });
        m_TimeLastFrame = std::chrono::system_clock::now();

        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "builtin_assets"), "/builtin_assets");
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "assets"), "/source_assets");
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "built"), "/assets");

#if SPARK_EDITOR
        m_EditorRuntime.Init();
#endif
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

#if SPARK_EDITOR
        m_EditorRuntime.Update();
#endif
    }
}
