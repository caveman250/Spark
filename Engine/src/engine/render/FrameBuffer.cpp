#include "FrameBuffer.h"
#include "engine/asset/texture/Texture.h"

namespace se::render
{
    FrameBuffer::FrameBuffer()
    {
        m_ColorTexture = asset::Texture::Create(1280, 720, asset::texture::Format::RGBA8, asset::texture::Usage::Read_RenderTarget);
        m_DepthTexture = asset::Texture::Create(1280, 720, asset::texture::Format::Depth16, asset::texture::Usage::RenderTarget);
    }
}