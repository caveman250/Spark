#import "MetalRenderer.h"
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "platform/mac/Window.h"
#import "engine/Application.h"


#if METAL_RENDERER

namespace se::render::metal
{
    MetalRenderer::MetalRenderer()
    {
    }

    MetalRenderer::~MetalRenderer()
    {
        [m_Device release];
        [m_CommandQueue release];
    }

    void MetalRenderer::Init()
    {
        m_Device = MTLCreateSystemDefaultDevice();
        m_CommandQueue = [m_Device newCommandQueue];
    }

    void MetalRenderer::Render()
    {
        auto commandBuffer= [m_CommandQueue commandBuffer];

        auto macWindow = static_cast<se::mac::Window*>(Application::Get()->GetPrimaryWindow());
        auto nativeWindow = macWindow->GetNativeWindow();
        auto view = [nativeWindow contentView];
        m_CurrentRenderPassDescriptor = [view currentRenderPassDescriptor];
        m_CommandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:m_CurrentRenderPassDescriptor];
        [m_CommandEncoder setCullMode:MTLCullModeBack];
        [m_CommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];

//        static std::shared_ptr<render::FrameBuffer> fb = render::FrameBuffer::CreateFrameBuffer();
//        PushFrameBuffer(fb);

        Renderer::Render();

        //PopFrameBuffer();

        [m_CommandEncoder endEncoding];
        [commandBuffer presentDrawable:[view currentDrawable]];
        [commandBuffer commit];
        m_CachedRenderState = {};
    }

    MTLRenderCommandEncoderPtr MetalRenderer::GetCurrentCommandEncoder() const
    {
        auto* frameBuffer = m_RenderGroups[m_ActiveRenderGroup].frameBuffer.get();
        if (frameBuffer)
        {
            return static_cast<FrameBuffer*>(frameBuffer)->GetRenderCommandEncoder();
        }
        return m_CommandEncoder;
    }
}
#endif
