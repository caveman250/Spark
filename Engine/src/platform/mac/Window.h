#pragma once

#include "spark.h"
#include "platform/IWindow.h"

#undef mac

namespace NS
{
    class Window;
}

namespace MTK
{
    class View;
}

namespace se::mac
{
    class Window : public IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window() override;

        void SetCurrent() override;
        void Cleanup() override;

        MTK::View* GetView() const { return m_View; };
        NS::Window* GetNSWindow() const { return m_NSWindow; };

    private:
        MTK::View* m_View;
        NS::Window* m_NSWindow;
    };
}
