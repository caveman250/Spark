#pragma once

#include "platform/PlatformRunLoop.h"

#if SPARK_PLATFORM_LINUX

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

#endif
