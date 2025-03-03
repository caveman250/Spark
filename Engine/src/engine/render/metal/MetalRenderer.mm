#import "MetalRenderer.h"
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "platform/mac/Window.h"


#if METAL_RENDERER

namespace se::render::metal
{
    MetalRenderer::MetalRenderer()
    {
    }

    MetalRenderer::~MetalRenderer()
    {
        [device release];
        [commandQueue release];
    }

    void MetalRenderer::Init()
    {
        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];
    }

    void MetalRenderer::Render(IWindow* window)
    {
        auto commandBuffer = [commandQueue commandBuffer];

        auto macWindow = static_cast<se::mac::Window*>(window);
        auto nativeWindow = macWindow->GetNativeWindow();
        auto view = [nativeWindow contentView];
        currentRenderPassDescriptor = [view currentRenderPassDescriptor];
        auto commandEncoder= [commandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor];
        currentCommandEncoder = commandEncoder;

        [currentCommandEncoder setCullMode:MTLCullModeBack];
        [currentCommandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];

        Renderer::Render(window);

        [currentCommandEncoder endEncoding];
        currentCommandEncoder = nullptr;
        [commandBuffer presentDrawable:[view currentDrawable]];
        [commandBuffer commit];
        m_CachedRenderState = {};
    }
}
#endif
