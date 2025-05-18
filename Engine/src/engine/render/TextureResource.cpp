#include "TextureResource.h"

#include "engine/asset/texture/Texture.h"

namespace se::render
{
    TextureResource::TextureResource(const asset::Texture& texture)
            : m_Width(texture.GetWidth()),
              m_Height(texture.GetHeight()),
              m_Format(texture.GetFormat()),
              m_Usage(texture.GetUsage()),
              m_MipData(texture.GetMips())
    {

    }
}
