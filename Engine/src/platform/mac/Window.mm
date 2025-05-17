#import "Window.h"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <AppKit/AppKit.h>
#import "engine/render/metal/MetalRenderer.h"
#import "platform/PlatformRunLoop.h"
#import "NativeWindow.h"

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
        MTKView* metalView = [[MTKView alloc] initWithFrame:frame device:renderer->GetDevice()];
        [metalView retain];

        [metalView setClearColor:MTLClearColorMake(0, 0, 0, 1)];
        [metalView setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
        [metalView setDepthStencilPixelFormat:MTLPixelFormatDepth16Unorm];
        [metalView setClearDepth:1.f];
        [metalView setDrawableSize:CGSize { static_cast<double>(resX), static_cast<double>(resY) } ];
        [metalView setPreferredFramesPerSecond: 240];
        m_ContentScale = [[metalView layer] contentsScale];

        NSRect viewport = [metalView bounds];
        NSRect backingSize = [metalView convertRectToBacking:viewport];

        m_ContentScale = backingSize.size.width / viewport.size.width;
        [[metalView layer] setContentsScale: m_ContentScale];
        [metalView setDrawableSize: NSSizeToCGSize(backingSize.size)];

        //[metalView addTrackingRect:frame owner:metalView userData:NULL assumeInside:NO];

        [m_Window setContentView:metalView];
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
