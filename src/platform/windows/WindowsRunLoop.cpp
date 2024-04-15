#include "WindowsRunLoop.h"

#include <algorithm>

#include "Window.h"
#include "engine/render/Renderer.h"
#include "platform/IWindow.h"

namespace se
{
    IRunLoop* IRunLoop::s_Instance = nullptr;

    IRunLoop* IRunLoop::CreatePlatformRunloop(std::vector<IWindow*> windows)
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

    void WindowsRunLoop::Tick()
    {
        for (const auto& window: m_Windows)
        {
            window->SetCurrent();

            render::Renderer::Get()->Render(window);

            MSG msg;
            while (PeekMessage(&msg, window->GetHWND(), 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            SwapBuffers(window->GetHDC());
        }

        render::Renderer::Get()->EndFrame();
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
    }
}
