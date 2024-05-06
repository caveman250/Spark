#pragma once

#include <engine/asset/texture/Mipmap.h>
#include <engine/asset/builder/TextureBlueprint.h>
#include "engine/asset/texture/Format.h"

namespace se::asset::importer
{
    struct DDSData
    {
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_MipCount;
        std::vector<texture::Mipmap> m_Mips;
        texture::Format m_Format;
    };

    DDSData ReadDDSData(const builder::CompressedImageData& rawDDSData);
}
