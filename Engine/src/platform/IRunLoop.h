#pragma once
#include "spark.h"

namespace se
{
    class IWindow;

    class IRunLoop
    {
    public:
        static IRunLoop* CreatePlatformRunloop(std::vector<IWindow*> windows);
        static IRunLoop* Get();
        virtual void Tick() = 0;
        virtual bool ShouldExit() = 0;
        virtual void RegisterWindow(IWindow* window) = 0;
        virtual void UnregisterWindow(IWindow* window) = 0;

    protected:
        static IRunLoop* s_Instance;
    };
}
