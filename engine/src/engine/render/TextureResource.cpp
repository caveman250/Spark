module;

#include "spark.h"

module Spark.Render.TextureResource;

namespace se::render
{
    TextureResource::TextureResource(size_t width,
        size_t height,
        asset::texture::Format format,
        asset::texture::Usage usage,
        const std::vector<asset::texture::Mipmap>& mips)
        : m_Width(width),
              m_Height(height),
              m_Format(format),
              m_Usage(usage),
              m_MipData(mips)
    {

    }
}
