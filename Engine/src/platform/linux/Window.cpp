
#include "Window.h"

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        return new linux::Window(resX, resY);
    }
}

namespace se::linux
{
    static std::unordered_map<SDL_Window*, Window*> s_WindowInstances;

    Window::Window(int resX, int resY)
        : IWindow(resX, resY)
    {
        uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        m_Window = SDL_CreateWindow("Spark", 0, 0, m_SizeX, m_SizeY, WindowFlags);
        m_Context = SDL_GL_CreateContext(m_Window);
        s_WindowInstances[m_Window] = this;
        SetCurrent();
    }

    Window::~Window()
    {
    }

    void Window::SetCurrent()
    {
        SDL_GL_MakeCurrent(m_Window, m_Context);
    }

    void Window::Cleanup()
    {
    }
}
