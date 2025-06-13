#pragma once

#include "spark.h"

namespace se::asset
{
    class Texture;
}

namespace se::render
{
    class FrameBuffer
    {
    public:
        FrameBuffer();
        virtual ~FrameBuffer() = default;
        static std::shared_ptr<FrameBuffer> CreateFrameBuffer();
        virtual void PreRender() = 0;
        virtual void Bind() = 0;
        virtual void UnBind() = 0;
        virtual void Commit() = 0;

        const std::shared_ptr<asset::Texture>& GetColorTexture() const { return m_ColorTexture; }
        const std::shared_ptr<asset::Texture>& GetDepthTexture() const { return m_DepthTexture; }

    protected:
        std::shared_ptr<asset::Texture> m_ColorTexture = nullptr;
        std::shared_ptr<asset::Texture> m_DepthTexture = nullptr;
    };
}
