#include "Window.h"
#include "platform/PlatformRunLoop.h"
#include "wrappers/Window.h"
import Spark.Render.Metal.MetalRenderer;
import Spark.Render.Renderer;

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        return new mac::Window(resX, resY);;
    }
}

namespace se::mac
{
    Window::Window(int resX, int resY)
            : IWindow(resX, resY)
    {
        m_Window = wrappers::Window::CreateWindow(resX, resY);

        auto renderer = render::Renderer::Get<render::metal::MetalRenderer>();
        m_View = wrappers::Window::CreateView(resX, resY, renderer->GetDevice());
        m_ContentScale = wrappers::Window::InitContentScale(m_View);
        wrappers::Window::AssignView(m_Window, m_View);
    }

    Window::~Window()
    {
        Cleanup();
    }

    void Window::Cleanup()
    {
        wrappers::Window::Cleanup(m_Window);
        m_Window = nullptr;
        m_View = nullptr;
    }
}
