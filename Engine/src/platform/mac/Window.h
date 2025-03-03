#pragma once

#include "spark.h"
#include "platform/IWindow.h"

#undef mac

#ifdef __OBJC__
@class NSWindow;
typedef NSWindow* NativeWindowPtr;
#else
typedef void* NativeWindowPtr;
#endif

namespace se::mac
{
    class Window : public IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window() override;

        void SetCurrent() override;
        void Cleanup() override;

        NativeWindowPtr GetNativeWindow() const { return m_Window; }

    private:
        NativeWindowPtr m_Window;
    };
}
