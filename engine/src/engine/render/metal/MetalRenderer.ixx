module;
#include "engine/render/render_fwd.h"
#include "MTL_fwd.h"

export module Spark.Render.Metal.MetalRenderer;
import Spark.Render.Renderer;

#if METAL_RENDERER

namespace se::render::metal
{
    export class MetalRenderer : public Renderer
    {
    public:
        MetalRenderer();
        ~MetalRenderer() override;
        RenderAPI GetRenderAPIType() const override { return RenderAPI::Metal; }
        void Init() override;
        void Render() override;
        bool SupportsMultiThreadedRendering() const override;

        void SetClearColour(const math::Vec4& colour);

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
