module;


module Spark.Render.FrameBuffer;
import Spark.Asset.Texture.Format;
import Spark.Asset.Texture;

namespace se::render
{
    FrameBuffer::FrameBuffer(const math::IntVec2& size)
        : m_Size(size)
    {
#if SPARK_PLATFORM_MAC
        m_ColorTexture = asset::Texture::Create(size.x, size.y, asset::texture::Format::BGRA8, asset::texture::Usage::Read_RenderTarget);
#else
        m_ColorTexture = asset::Texture::Create(size.x, size.y, asset::texture::Format::RGBA8, asset::texture::Usage::Read_RenderTarget);
#endif
        m_DepthTexture = asset::Texture::Create(size.x, size.y, asset::texture::Format::Depth16, asset::texture::Usage::RenderTarget);
    }
}
