#pragma once
#include "platform/PlatformRunLoop.h"

namespace se::windows
{
    class Window;

    class WindowsRunLoop : public PlatformRunLoop
    {
    public:
        WindowsRunLoop();
        void Run() override;
        void Update() override;
        bool ShouldExit() override;
    private:
        bool m_ShouldExit = false;
    };
}
