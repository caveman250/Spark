#include "Window.h"

#include "engine/render/Renderer.h"
#include "engine/render/metal/MetalRenderer.h"
#include "engine/render/metal/MTL_fwd.h"
#include "platform/PlatformRunLoop.h"

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        auto* window = new mac::Window(resX, resY);
        if (auto runLoop = PlatformRunLoop::Get())
        {
            runLoop->RegisterWindow(window);
        }

        return window;
    }
}

namespace se::mac
{
    Window::Window(int resX, int resY)
        : IWindow(resX, resY)
    {
        CGRect frame = (CGRect){{0.0, 0.0}, {static_cast<double>(resX), static_cast<double>(resY)}};

        m_NSWindow = NS::Window::alloc()->init(
            frame,
            NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
            NS::BackingStoreBuffered,
            false);

        auto renderer = render::Renderer::Get<render::metal::MetalRenderer>();

        m_View = MTK::View::alloc()->init(frame, renderer->GetDevice());
        m_View->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
        m_View->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
        m_View->setDepthStencilPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm); // TODO
        m_View->setClearDepth(1.0f);
        m_View->setDrawableSize(CGSize { static_cast<double>(resX), static_cast<double>(resY) }); // TODO content scale.

        m_NSWindow->setContentView(m_View);
        m_NSWindow->setTitle(NS::String::string("Spark", NS::StringEncoding::UTF8StringEncoding));

        m_NSWindow->makeKeyAndOrderFront(nullptr);
    }

    Window::~Window()
    {
        Cleanup();
    }

    void Window::SetCurrent()
    {
        // TODO
    }

    void Window::Cleanup()
    {
        m_View->release();
        m_NSWindow->release();
    }
}
