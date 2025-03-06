#pragma once

#include "spark.h"
#include "platform/IWindow.h"
#include "engine/render/metal/MTL_fwd.h"

#undef mac

namespace se::mac
{
    class Window : public IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window() override;

        void Cleanup() override;

        NSWindowPtr GetNativeWindow() const { return m_Window; }

    private:
        NSWindowPtr m_Window;
    };
}
