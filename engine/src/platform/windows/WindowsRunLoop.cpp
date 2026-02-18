
#include "WindowsRunLoop.h"

#include "platform/PlatformRunLoop.h"


#include "Window.h"
#include "engine/Application.h"
#include <easy/profiler.h>
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

    void WindowsRunLoop::Init()
    {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        PlatformRunLoop::Init();
    }

    void WindowsRunLoop::Run()
    {
        Init();
        while (!ShouldExit())
        {
            Update();
        }
    }

    void WindowsRunLoop::Update()
    {
        Window* window = static_cast<Window*>(m_Window);
        EASY_BLOCK("WindowsRunLoop::Update");
        {
            EASY_BLOCK("Process Messages")
            MSG msg;
            while (PeekMessage(&msg, window->GetHWND(), 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        Application* app = Application::Get();
        const auto input = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
        input::InputComponent& temp_input = window->GetTempInputComponent();
        temp_input.lastMouseX = input->lastMouseX;
        temp_input.lastMouseY = input->lastMouseY;
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
        EASY_BLOCK("SwapBuffers");
        SwapBuffers(window->GetHDC());
        EASY_END_BLOCK

        render::Renderer::Get<render::Renderer>()->EndFrame();
    }

    bool WindowsRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }
}
