#pragma once

#include "spark.h"
#include "platform/IWindow.h"

#undef linux

namespace se::linux
{
    class Window : public IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window() override;

        void SetCurrent() override;
        void Cleanup() override;

        SDL_Window* GetSDLWindow() const { return m_Window; }
    private:
        SDL_Window* m_Window = nullptr;
        SDL_GLContext m_Context = {};
    };
}
