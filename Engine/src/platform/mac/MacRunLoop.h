#pragma once
#include "platform/PlatformRunLoop.h"

namespace se::mac
{
    class Window;

    class MacRunLoop : public PlatformRunLoop
    {
    public:
        void Run() override;
        void Update() override;
        void Shutdown() override;
        bool ShouldExit() override;
    private:
        bool m_ShouldExit = false;
    };
}
