#include "Window.h"
#import "platform/mac/NativeWindow.h"

namespace se::mac::wrappers
{
    NSWindowPtr Window::CreateWindow(int x, int y)
    {
        NSRect frame = NSMakeRect(0, 0, x, y);

        NSUInteger styleMask =
                NSWindowStyleMaskTitled |
                NSWindowStyleMaskResizable |
                NSWindowStyleMaskClosable |
                NSWindowStyleMaskMiniaturizable;

        NSBackingStoreType backing = NSBackingStoreBuffered;

        NSWindowPtr window = [[NativeWindow alloc] initWithContentRect:frame styleMask:styleMask backing:backing defer:NO];
        [(NativeWindow*)window initKeyReceiver];
        [window setAcceptsMouseMovedEvents:YES];
        //[m_Window setTitlebarAppearsTransparent: true];
        //NSColor* bg = [NSColor colorWithCalibratedRed:0.13f green:0.13f blue:0.13f alpha:1.0f];
        //[m_Window setBackgroundColor: bg];
        [window setAppearance: [NSAppearance appearanceNamed:NSAppearanceNameVibrantDark]];
        [window center];

        return window;
    }

    SparkViewPtr Window::CreateView(int x, int y, MTLDevicePtr device)
    {
        NSRect frame = NSMakeRect(0, 0, x, y);

        SparkViewPtr view = [[SparkView alloc] initWithFrame:frame device:device];
        [view retain];

        [view createTrackingArea];
        [view setCurrentCursor:[NSCursor arrowCursor]];

        [view setClearColor:MTLClearColorMake(0, 0, 0, 1)];
        [view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [view setDepthStencilPixelFormat:MTLPixelFormatDepth16Unorm];
        [view setClearDepth:1.f];
        [view setDrawableSize:CGSize { static_cast<double>(x), static_cast<double>(y) } ];
        [view setPreferredFramesPerSecond: 240];

        return view;
    }

    float Window::InitContentScale(SparkViewPtr view)
    {
        NSRect viewport = [view bounds];
        NSRect backingSize = [view convertRectToBacking:viewport];

        float contentScale = backingSize.size.width / viewport.size.width;
        [[view layer] setContentsScale: contentScale];
        [view setDrawableSize: NSSizeToCGSize(backingSize.size)];

        return contentScale;
    }

    void Window::AssignView(NSWindowPtr window, SparkViewPtr view)
    {
        [window setContentView:view];
        [window setTitle:[[NSString alloc] initWithUTF8String:"Spark"]];
        [window makeKeyAndOrderFront:window];
        [window retain];
    }

    void Window::Cleanup(NSWindowPtr window)
    {
        auto view = [window contentView];
        [view release];
        [window release];
    }
}
