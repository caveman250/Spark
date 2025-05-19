#pragma once

#include "spark.h"
#include "platform/PlatformRunLoop.h"

#undef linux

namespace se::linux
{
    class Window;
    class LinuxRunLoop : public PlatformRunLoop
    {
    public:
        LinuxRunLoop();
        void Update() override;
        void Run() override;
        bool ShouldExit() override;
    private:
        bool m_ShouldExit = false;
    };
}
