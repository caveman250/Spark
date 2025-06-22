#pragma once

#include "engine/render/FrameBuffer.h"
#include "MTL_fwd.h"

#if METAL_RENDERER

namespace se::math
{
    struct IntVec2;
}

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

    private:
        void PreRender() override;
        void Bind() override;
        void UnBind() override;
        void Commit() override;

        MTLCommandBufferPtr m_CommandBuffer = nullptr;
        MTLRenderCommandEncoderPtr m_RenderCommandEncoder = nullptr;
    };
}

#endif
