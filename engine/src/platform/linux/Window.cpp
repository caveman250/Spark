
#include "Window.h"

#include "engine/Application.h"
#include "platform/PlatformRunLoop.h"

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        return new linux::Window(resX, resY);;
    }
}

namespace se::linux
{
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
    }

    Window::~Window()
    {
        SDL_DestroyWindow(m_Window);
    }

    void Window::Cleanup()
    {
    }
}
