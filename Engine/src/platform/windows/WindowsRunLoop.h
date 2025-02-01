#pragma once
#include "platform/PlatformRunLoop.h"

namespace se::windows
{
    class Window;

    class WindowsRunLoop : public PlatformRunLoop
    {
    public:
        WindowsRunLoop(std::vector<IWindow*> windows);
        void Run() override;
        void Update() override;
        bool ShouldExit() override;
        void RegisterWindow(IWindow *window) override;
        void UnregisterWindow(IWindow *window) override;
    private:
        std::vector<Window*> m_Windows;
        bool m_ShouldExit = false;
    };
}
