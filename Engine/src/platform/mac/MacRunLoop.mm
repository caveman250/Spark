#include "MacRunLoop.h"

#include "platform/PlatformRunLoop.h"

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "platform/mac/Window.h"
#import <AppKit/AppKit.h>
#import "AppDelegate.h"

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
        @autoreleasepool {
            NSApplication* app = [NSApplication sharedApplication];
            AppDelegate* appDelegate = [[AppDelegate alloc] init];

            [app setDelegate:appDelegate];

            [app run];
        }
    }

    void MacRunLoop::Update()
    {
        PROFILE_SCOPE("MacRunLoop::Update")

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
}
