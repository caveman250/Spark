#pragma once

#include "spark.h"
#include "engine/math/math.h"

namespace se::asset
{
    class Texture;
}

namespace se::render
{
    class FrameBuffer
    {
    public:
        explicit FrameBuffer(const math::IntVec2& size);
        virtual ~FrameBuffer() = default;
        static std::shared_ptr<FrameBuffer> CreateFrameBuffer(const math::IntVec2& size);
        virtual void PreRender() = 0;
        virtual void Bind() = 0;
        virtual void UnBind() = 0;
        virtual void Commit() = 0;

        const math::IntVec2& GetSize() const { return m_Size; }

        const std::shared_ptr<asset::Texture>& GetColorTexture() const { return m_ColorTexture; }
        const std::shared_ptr<asset::Texture>& GetDepthTexture() const { return m_DepthTexture; }

    protected:
        math::IntVec2 m_Size;

        std::shared_ptr<asset::Texture> m_ColorTexture = nullptr;
        std::shared_ptr<asset::Texture> m_DepthTexture = nullptr;
    };
}
