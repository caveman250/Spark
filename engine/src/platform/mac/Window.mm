#import "Window.h"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <AppKit/AppKit.h>
#import "engine/render/metal/MetalRenderer.h"
#import "platform/PlatformRunLoop.h"
#import "NativeWindow.h"
#import "AppDelegate.h"

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        return new mac::Window(resX, resY);;
    }
}

namespace se::mac
{
    Window::Window(int resX, int resY)
            : IWindow(resX, resY)
    {
        NSRect frame = NSMakeRect(0, 0, resX, resY);

        NSUInteger styleMask =
                NSWindowStyleMaskTitled |
                NSWindowStyleMaskResizable |
                NSWindowStyleMaskClosable |
                NSWindowStyleMaskMiniaturizable;

        NSBackingStoreType backing = NSBackingStoreBuffered;

        m_Window = [[NativeWindow alloc] initWithContentRect:frame styleMask:styleMask backing:backing defer:NO];
        [(NativeWindow*)m_Window initKeyReceiver];
        [m_Window setAcceptsMouseMovedEvents:YES];
        [m_Window center];

        auto renderer = se::render::Renderer::Get<se::render::metal::MetalRenderer>();
        m_View = [[SparkView alloc] initWithFrame:frame device:renderer->GetDevice()];
        [m_View retain];

        [m_View createTrackingArea];
        [m_View setCurrentCursor:[NSCursor arrowCursor]];

        [m_View setClearColor:MTLClearColorMake(0, 0, 0, 1)];
        [m_View setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [m_View setDepthStencilPixelFormat:MTLPixelFormatDepth16Unorm];
        [m_View setClearDepth:1.f];
        [m_View setDrawableSize:CGSize { static_cast<double>(resX), static_cast<double>(resY) } ];
        [m_View setPreferredFramesPerSecond: 240];
        m_ContentScale = [[m_View layer] contentsScale];

        NSRect viewport = [m_View bounds];
        NSRect backingSize = [m_View convertRectToBacking:viewport];

        m_ContentScale = backingSize.size.width / viewport.size.width;
        [[m_View layer] setContentsScale: m_ContentScale];
        [m_View setDrawableSize: NSSizeToCGSize(backingSize.size)];

        [m_Window setContentView:m_View];
        [m_Window setTitle:[[NSString alloc] initWithUTF8String:"Spark"]];
        [m_Window makeKeyAndOrderFront:m_Window];
        [m_Window retain];
    }

    Window::~Window()
    {
        Cleanup();
    }

    void Window::Cleanup()
    {
        auto view = [m_Window contentView];
        [view release];
        [m_Window release];
    }
}
