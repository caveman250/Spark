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

    PlatformRunLoop* PlatformRunLoop::CreatePlatformRunloop(std::vector<IWindow*> windows)
    {
        s_Instance = new mac::MacRunLoop(windows);
        return s_Instance;
    }
}

namespace se::mac
{
    MacRunLoop::MacRunLoop(std::vector<IWindow*> windows)
    {
        std::ranges::for_each(windows, [this](IWindow* window)
        {
            RegisterWindow(window);
        });
    }

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

        auto safeCopy = m_Windows;
        for (const auto& window: safeCopy)
        {
            if (window->ShouldClose())
            {
                window->Cleanup();
                delete window;
            }
        }

        if (ShouldExit())
        {
            return;
        }

        PlatformRunLoop::Update();

        render::Renderer* renderer = render::Renderer::Get<render::Renderer>();
        for (const auto& window: m_Windows)
        {
            window->SetCurrent();
            renderer->Render(window);
        }

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

    void MacRunLoop::RegisterWindow(IWindow* window)
    {
        Window* platformWindow = dynamic_cast<Window*>(window);
        SPARK_ASSERT(platformWindow);
        m_Windows.push_back(platformWindow);
    }

    void MacRunLoop::UnregisterWindow(IWindow* window)
    {
        std::erase(m_Windows, window);
        if (m_Windows.empty() || window == Application::Get()->GetPrimaryWindow())
        {
            m_ShouldExit = true;
        }
    }
}
