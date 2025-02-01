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
        LinuxRunLoop(std::vector<IWindow*> windows);
        void Update() override;
        void Run() override;
        bool ShouldExit() override;
        void RegisterWindow(IWindow *window) override;
        void UnregisterWindow(IWindow *window) override;
    private:
        std::unordered_map<uint32_t, Window*> m_Windows;
        bool m_ShouldExit = false;
    };
}
