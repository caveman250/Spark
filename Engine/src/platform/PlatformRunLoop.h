#pragma once
#include "spark.h"

namespace se
{
    class IWindow;

    class PlatformRunLoop
    {
    public:
        static PlatformRunLoop* CreatePlatformRunloop(std::vector<IWindow*> windows);
        static PlatformRunLoop* Get();
        virtual void Update();
        virtual bool ShouldExit() = 0;
        virtual void RegisterWindow(IWindow* window) = 0;
        virtual void UnregisterWindow(IWindow* window) = 0;

    protected:
        static PlatformRunLoop* s_Instance;
    };
}
