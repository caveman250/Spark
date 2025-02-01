
#include "Window.h"

#include "engine/Application.h"
#include "platform/PlatformRunLoop.h"

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        auto* window = new mac::Window(resX, resY);
        if (auto runLoop = Application::Get()->GetRunLoop())
        {
            runLoop->RegisterWindow(window);
        }

        return window;
    }
}

namespace se::mac
{
    Window::Window(int resX, int resY)
        : IWindow(resX, resY)
    {
        // TODO
    }

    Window::~Window()
    {
        // TODO
    }

    void Window::SetCurrent()
    {
       // TODO
    }

    void Window::Cleanup()
    {
      // TODO
    }
}
