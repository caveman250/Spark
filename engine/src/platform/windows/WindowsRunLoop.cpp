
#include "WindowsRunLoop.h"

#include "platform/PlatformRunLoop.h"


#include "Window.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "platform/IWindow.h"

namespace se
{
    PlatformRunLoop* PlatformRunLoop::s_Instance = nullptr;

    PlatformRunLoop* PlatformRunLoop::CreatePlatformRunloop()
    {
        s_Instance = new windows::WindowsRunLoop();
        return s_Instance;
    }
}

namespace se::windows
{
    WindowsRunLoop::WindowsRunLoop()
    {
    }

    void WindowsRunLoop::Run()
    {
        Init();
        while (!ShouldExit())
        {
            Update();
            PROFILE_BEGIN_FRAME()
            PROFILE_BEGIN_THREAD()
        }
    }

    void WindowsRunLoop::Update()
    {
        Window* window = static_cast<Window*>(m_Window);
        PROFILE_SCOPE("WindowsRunLoop::Update")
        {
            PROFILE_SCOPE("Process Messages")
            MSG msg;
            while (PeekMessage(&msg, window->GetHWND(), 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        Application* app = Application::Get();
        auto input = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
        input::InputComponent& temp_input = window->GetTempInputComponent();
        *input = temp_input;
        temp_input.keyEvents.clear();
        temp_input.mouseEvents.clear();

        if (m_Window->ShouldClose())
        {
            m_Window->Cleanup();
            delete m_Window;
            m_ShouldExit = true;
        }

        if (ShouldExit())
        {
            return;
        }

        PlatformRunLoop::Update();

        render::Renderer::Get<render::Renderer>()->Render();
        SwapBuffers(window->GetHDC());

        render::Renderer::Get<render::Renderer>()->EndFrame();
    }

    bool WindowsRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }
}
