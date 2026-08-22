module;

#include "MTL_fwd.h"

#if METAL_RENDERER

export module Spark.Render.Metal.FrameBuffer;
import Spark.Render.FrameBuffer;
import Spark.Math;

namespace se::asset
{
    class Texture;
}

namespace se::render::metal
{
    class FrameBuffer : public render::FrameBuffer
    {
    public:
        FrameBuffer(const math::IntVec2&);
        MTLRenderCommandEncoderPtr GetRenderCommandEncoder() const
        {
            return m_RenderCommandEncoder;
        }

        void SetClearColour(const math::Vec4& colour);

    private:
        void PreRender() override;
        void Bind() override;
        void UnBind() override;
        void Commit() override;

        MTLCommandBufferPtr m_CommandBuffer = nullptr;
        MTLRenderCommandEncoderPtr m_RenderCommandEncoder = nullptr;
        MTLRenderPassDescriptorPtr m_CurrentRenderPassDescriptor = nullptr;
        MTLRenderPassColorAttachmentDescriptorPtr m_ColourAttachmentDescriptor = nullptr;
        math::Vec4 m_ClearColour = {};
    };
}

#endif
