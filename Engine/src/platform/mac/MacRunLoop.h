#pragma once
#include "platform/PlatformRunLoop.h"

namespace se::mac
{
    class Window;

    class MacRunLoop : public PlatformRunLoop
    {
    public:
        MacRunLoop(std::vector<IWindow*> windows);
        void Update() override;
        bool ShouldExit() override;
        void RegisterWindow(IWindow *window) override;
        void UnregisterWindow(IWindow *window) override;
    private:
        std::vector<Window*> m_Windows;
        bool m_ShouldExit = false;
    };
}
