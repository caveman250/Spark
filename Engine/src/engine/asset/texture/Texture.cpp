#include "Texture.h"

#include "Format.h"
#include "Mipmap.h"
#include "engine/asset/binary/binary.h"
#include "engine/asset/binary/Database.h"
#include "engine/asset/builder/TextureBlueprint.h"
#include "engine/render/TextureResource.h"

namespace se::asset
{
DEFINE_SPARK_CLASS_BEGIN(Texture)
    DEFINE_SERIALIZED_MEMBER(m_Width)
    DEFINE_SERIALIZED_MEMBER(m_Height)
    DEFINE_SERIALIZED_MEMBER(m_MipCount)
    DEFINE_SERIALIZED_MEMBER(m_Mips)
    DEFINE_SERIALIZED_MEMBER(m_Format)
    DEFINE_SERIALIZED_MEMBER(m_Usage)
DEFINE_SPARK_CLASS_END(Texture)

    Texture::Texture()
    {
    }

    Texture::Texture(uint32_t width,
            uint32_t height,
            texture::Format::Type format,
            texture::Usage::Type usage)
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

    std::shared_ptr<Texture> Texture::FromRawData(uint32_t width, uint32_t height, const memory::BinaryBlob &blob, texture::Format::Type format)
    {
        std::shared_ptr<Texture> ret = std::make_shared<Texture>();
        ret->m_Width = width;
        ret->m_Height = height;
        ret->m_Mips.push_back(texture::Mipmap(blob, width, height));
        ret->m_Format = format;
        ret->m_MipCount = 1;
        return ret;
    }

    std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height, texture::Format::Type format, texture::Usage::Type usage)
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
