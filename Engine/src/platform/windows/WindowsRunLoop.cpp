
#include "WindowsRunLoop.h"

#include <algorithm>
#include "platform/PlatformRunLoop.h"


#include "Window.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "platform/IWindow.h"

namespace se
{
    PlatformRunLoop* PlatformRunLoop::s_Instance = nullptr;

    PlatformRunLoop* PlatformRunLoop::CreatePlatformRunloop(std::vector<IWindow*> windows)
    {
        s_Instance = new windows::WindowsRunLoop(windows);
        return s_Instance;
    }
}

namespace se::windows
{
    WindowsRunLoop::WindowsRunLoop(std::vector<IWindow*> windows)
    {
        std::ranges::for_each(windows, [this](IWindow* window)
        {
            RegisterWindow(window);
        });
    }

    void WindowsRunLoop::Update()
    {
        PROFILE_SCOPE("WindowsRunLoop::Update")
        for (const auto& window: m_Windows)
        {
            PROFILE_SCOPE("Process Messages")
            MSG msg;
            while (PeekMessage(&msg, window->GetHWND(), 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        auto safeCopy = m_Windows;
        for (const auto& window: safeCopy)
        {
            if (window->ShouldClose())
            {
                window->Cleanup();
                delete window;
            }
        }

        if (ShouldExit())
        {
            return;
        }

        PlatformRunLoop::Update();

        for (const auto& window: m_Windows)
        {
            window->SetCurrent();
            render::Renderer::Get()->Render(window);
            SwapBuffers(window->GetHDC());
        }

        render::Renderer::Get()->EndFrame();
    }

    bool WindowsRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }

    void WindowsRunLoop::RegisterWindow(IWindow* window)
    {
        Window* platformWindow = dynamic_cast<Window*>(window);
        SPARK_ASSERT(platformWindow);
        m_Windows.push_back(platformWindow);
    }

    void WindowsRunLoop::UnregisterWindow(IWindow* window)
    {
        std::erase(m_Windows, window);
        if (m_Windows.empty() || window == Application::Get()->GetPrimaryWindow())
        {
            m_ShouldExit = true;
        }
    }
}
