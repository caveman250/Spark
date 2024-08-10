
#include "Window.h"

#include "engine/Application.h"
#include "platform/PlatformRunLoop.h"

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        auto* window = new linux::Window(resX, resY);
        if (auto runLoop = Application::Get()->GetRunLoop())
        {
            runLoop->RegisterWindow(window);
        }

        return window;
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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#if SPARK_DEBUG
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
        m_Context = SDL_GL_CreateContext(m_Window);
        SDL_GL_SetSwapInterval(0);
        if (auto primaryWindow = Application::Get()->GetPrimaryWindow())
        {
            // SDL_GL_CreateContext sets the new context as current.
            // keep the primary window as the default and make users opt in to context switches.
            primaryWindow->SetCurrent();
        }
        s_WindowInstances[m_Window] = this;
    }

    Window::~Window()
    {
        s_WindowInstances.erase(m_Window);
        if (auto platformRunLoop = PlatformRunLoop::Get())
        {
            platformRunLoop->UnregisterWindow(this);
        }
        SDL_DestroyWindow(m_Window);
    }

    void Window::SetCurrent()
    {
        SDL_GL_MakeCurrent(m_Window, m_Context);
    }

    void Window::Cleanup()
    {
    }
}
