#include "MacRunLoop.h"

#include "platform/PlatformRunLoop.h"

#include <easy/profiler.h>
#include "platform/mac/Window.h"
#import <AppKit/AppKit.h>
#import "AppDelegate.h"
#include "wrappers/RunLoop.h"
import Spark.Render.Renderer;

namespace se
{
    PlatformRunLoop* PlatformRunLoop::s_Instance = nullptr;

    PlatformRunLoop* PlatformRunLoop::CreatePlatformRunloop()
    {
        s_Instance = new mac::MacRunLoop();
        return s_Instance;
    }
}

namespace se::mac
{
    void MacRunLoop::Run()
    {
        wrappers::RunLoop::Run();
    }

    void MacRunLoop::Update()
    {
        EASY_BLOCK("MacRunLoop::Update");

        if (m_Window->ShouldClose())
        {
            m_Window->Cleanup();
            delete m_Window;
        }

        if (ShouldExit())
        {
            return;
        }

        PlatformRunLoop::Update();

        render::Renderer* renderer = render::Renderer::Get<render::Renderer>();
        renderer->Render();

        renderer->EndFrame();
    }

    void MacRunLoop::Shutdown()
    {
        PlatformRunLoop::Shutdown();
    }

    bool MacRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }

    void MacRunLoop::RequestExit()
    {
        PlatformRunLoop::RequestExit();
        m_ShouldExit = true;

        wrappers::RunLoop::Shutdown();
    }
}
