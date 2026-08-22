module;

#include "easy/profiler.h"
#include <memory>

module Spark.Asset.Texture;
import Spark.Asset.Texture.Format;
import Spark.Asset.Meta.MetaData;
import Spark.Asset.Meta.TextureMetaData;
import Spark.Asset.Texture.Mipmap;
import Spark.Memory.BinaryBlob;
import Spark.Render.TextureResource;
import Spark.Render.Util.TextureUtil;

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

    std::shared_ptr<meta::MetaData> Texture::CreateMetaData() const
    {
        return std::make_shared<meta::TextureMetaData>(m_Path);
    }

    void Texture::Release()
    {
        for (auto& mip : m_Mips)
        {
            mip.m_Data.Release();
        }
    }

    std::shared_ptr<Texture> Texture::FromRawData(uint32_t width, uint32_t height, const memory::BinaryBlob &blob, texture::Format format, texture::Usage usage)
    {
        std::shared_ptr<Texture> ret = std::make_shared<Texture>();
        ret->m_Width = width;
        ret->m_Height = height;
        ret->m_Mips.push_back(texture::Mipmap(blob, width, height));
        ret->m_Format = format;
        ret->m_MipCount = 1;
        ret->m_Usage = usage;
        return ret;
    }

    std::shared_ptr<Texture> Texture::Create(uint32_t width, uint32_t height, texture::Format format, texture::Usage usage)
    {
        return std::shared_ptr<Texture>(new Texture(width, height, format, usage));
    }

    const std::shared_ptr<render::TextureResource> &Texture::GetPlatformResource()
    {
        EASY_FUNCTION()
        if (!m_PlatformResource)
        {
            m_PlatformResource = render::util::TextureUtil::CreateTextureResource(*this);
        }

        if (!m_PlatformResource->HasCreatedPlatformResources())
        {
            m_PlatformResource->CreatePlatformResources();
        }
        return m_PlatformResource;
    }
}
