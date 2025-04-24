#pragma once

#include "spark.h"
#include "engine/render/FrameBuffer.h"
#include "MTL_fwd.h"

namespace se::asset
{
    class Texture;
}

namespace se::render::metal
{
    class FrameBuffer : public render::FrameBuffer
    {
    public:
        FrameBuffer();

        MTLRenderCommandEncoderPtr GetRenderCommandEncoder() const
        {
            return m_RenderCommandEncoder;
        }

    private:
        void Bind() override;
        void Commit() override;

        std::shared_ptr<asset::Texture> m_ColorTexture = nullptr;
        std::shared_ptr<asset::Texture> m_DepthTexture = nullptr;
        MTLCommandBufferPtr m_CommandBuffer = nullptr;
        MTLRenderCommandEncoderPtr m_RenderCommandEncoder = nullptr;
    };
}
