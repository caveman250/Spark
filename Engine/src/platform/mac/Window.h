#pragma once

#include "spark.h"
#include "platform/IWindow.h"

#undef mac

namespace se::mac
{
    class Window : public IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window() override;

        void SetCurrent() override;
        void Cleanup() override;
    };
}
