#pragma once
#include "platform/IRunLoop.h"

namespace se::windows
{
    class Window;

    class WindowsRunLoop : public IRunLoop
    {
    public:
        WindowsRunLoop(std::vector<IWindow*> windows);
        void Tick() override;
        void RegisterWindow(IWindow *window) override;
        void UnregisterWindow(IWindow *window) override;
    private:
       std::vector<Window*> m_Windows;
    };
}
