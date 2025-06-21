#pragma once

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

        SparkViewPtr GetView() const { return m_View; }
        NSWindowPtr GetNativeWindow() const { return m_Window; }

    private:
        SparkViewPtr m_View = nullptr;
        NSWindowPtr m_Window = nullptr;
    };
}
