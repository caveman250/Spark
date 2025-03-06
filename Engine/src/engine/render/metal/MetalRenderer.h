#pragma once
#include "engine/render/render_fwd.h"
#include "MTL_fwd.h"

#if METAL_RENDERER
#include "engine/render/Renderer.h"

namespace se::render::metal
{
    class MetalRenderer : public Renderer
    {
    public:
        MetalRenderer();
        ~MetalRenderer() override;
        RenderAPI::Type GetRenderAPIType() const override { return RenderAPI::Metal; }
        void Init() override;
        void Render() override;

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
