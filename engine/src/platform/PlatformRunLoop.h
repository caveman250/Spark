#pragma once

namespace se
{
    class IWindow;

    class PlatformRunLoop
    {
    public:
        static PlatformRunLoop* CreatePlatformRunloop();
        static PlatformRunLoop* Get();

        virtual ~PlatformRunLoop() {}
        virtual void Init();
        virtual void Run() = 0;
        virtual void Shutdown();
        virtual void Update();
        virtual bool ShouldExit() = 0;

    protected:
        static PlatformRunLoop* s_Instance;
        IWindow* m_Window;
    };
}
