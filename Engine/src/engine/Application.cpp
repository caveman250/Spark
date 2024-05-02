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

        io::VFS::Get().Mount("../../Engine/builtin_assets", "/builtin_assets"); //todo make this more robust

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
#if SPARK_EDITOR
        m_EditorRuntime.Update();
#endif
    }
}
