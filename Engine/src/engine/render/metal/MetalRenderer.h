#pragma once
#include "MTL_fwd.h"
#include "engine/render/render_fwd.h"

#if METAL_RENDERER
#include "engine/render/Renderer.h"

#ifdef __OBJC__
@protocol MTLDevice;
typedef id<MTLDevice> MTLDevicePtr;
@protocol MTLRenderCommandEncoder;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
@protocol MTLCommandQueue;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
@class MTLRenderPassDescriptor;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
#else
typedef void NativeRenderer;
typedef void* MTLDevicePtr;
typedef void* MTLRenderCommandEncoderPtr;
typedef void* MTLCommandQueuePtr;
typedef void* MTLRenderPassDescriptorPtr;
#endif

namespace se::render::metal
{
    class MetalRenderer : public Renderer
    {
    public:
        MetalRenderer();
        ~MetalRenderer() override;
        RenderAPI::Type GetRenderAPIType() const override { return RenderAPI::Metal; }
        void Init() override;
        void Render(IWindow* window) override;

        MTLDevicePtr GetDevice() const { return device; }
        MTLRenderCommandEncoderPtr GetCurrentCommandEncoder() const { return currentCommandEncoder; }

    private:
        MTLDevicePtr device;
        MTLCommandQueuePtr commandQueue;
        MTLRenderCommandEncoderPtr currentCommandEncoder;
        MTLRenderPassDescriptorPtr currentRenderPassDescriptor;
    };
}
#endif
