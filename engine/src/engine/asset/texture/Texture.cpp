#include "Texture.h"

#include "Format.h"
#include "Mipmap.h"
#include "engine/asset/binary/binary.h"
#include "engine/asset/binary/Database.h"
#include "engine/asset/builder/TextureBlueprint.h"
#include "engine/render/TextureResource.h"

namespace se::asset
{
    Texture::Texture()
    {
    }

    Texture::Texture(uint32_t width,
            uint32_t height,
            texture::Format format,
            texture::Usage usage)
            : m_Width(width)
            , m_Height(height)
            , m_MipCount(0)
            , m_Format(format)
            , m_Usage(usage)
    {}

    Texture::~Texture()
    {
        Release();
    }

    void Texture::Release()
    {
        for (auto& mip : m_Mips)
        {
            mip.m_Data.Release();
        }
    }

    std::shared_ptr<Texture> Texture::FromRawData(uint32_t width, uint32_t height, const memory::BinaryBlob &blob, texture::Format format)
    {
        std::shared_ptr<Texture> ret = std::make_shared<Texture>();
        ret->m_Width = width;
        ret->m_Height = height;
        ret->m_Mips.push_back(texture::Mipmap(blob, width, height));
        ret->m_Format = format;
        ret->m_MipCount = 1;
        return ret;
    }

    std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height, texture::Format format, texture::Usage usage)
    {
        return std::shared_ptr<Texture>(new Texture(width, height, format, usage));
    }

    const std::shared_ptr<render::TextureResource> &Texture::GetPlatformResource()
    {
        if (!m_PlatformResource)
        {
            m_PlatformResource = render::TextureResource::Create(*this);
        }

        if (!m_PlatformResource->HasCreatedPlatformResources())
        {
            m_PlatformResource->CreatePlatformResources();
        }
        return m_PlatformResource;
    }
}
