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
        RenderAPI GetRenderAPIType() const override { return RenderAPI::Metal; }
        void Init() override;
        void Render() override;

        MTLDevicePtr GetDevice() const { return m_Device; }
        MTLRenderCommandEncoderPtr GetCurrentCommandEncoder() const;
        MTLCommandQueuePtr GetCommandQueue() const { return m_CommandQueue; }

    private:
        MTLDevicePtr m_Device = nullptr;
        MTLCommandQueuePtr m_CommandQueue = nullptr;
        MTLRenderCommandEncoderPtr m_CommandEncoder = nullptr;
        MTLRenderPassDescriptorPtr m_CurrentRenderPassDescriptor = nullptr;
    };
}
#endif
