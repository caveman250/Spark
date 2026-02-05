#pragma once

#include "Format.h"
#include "engine/asset/Asset.h"
#include "engine/asset/meta/TextureMetaData.h"
#include "engine/asset/texture/Mipmap.h"
#include "engine/render/TextureResource.h"

namespace se::render
{
    class TextureResource;
}

namespace se::asset::builder
{
    struct CompressedImageData;
}

namespace se::asset
{
    class Texture : public Asset
    {
        SPARK_CLASS()

    public:
        Texture();
        ~Texture() override;

        bool UsesMetaData() const override { return true; }
        std::shared_ptr<meta::MetaData> CreateMetaData() const override;

        void Release();
        static std::shared_ptr<Texture> FromRawData(uint32_t width, uint32_t height, const memory::BinaryBlob& blob, texture::Format format, texture::Usage usage);
        static std::shared_ptr<Texture> Create(uint32_t width, uint32_t height, texture::Format format, texture::Usage usage);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetMipCount() const { return m_MipCount; }
        const std::vector<texture::Mipmap>& GetMips() const { return m_Mips; }
        texture::Format GetFormat() const { return m_Format; }
        texture::Usage GetUsage() const { return m_Usage; }

        const std::shared_ptr<render::TextureResource>& GetPlatformResource();

    private:
        Texture(uint32_t width, uint32_t height, texture::Format format, texture::Usage usage);

        SPARK_MEMBER(Serialized)
        uint32_t m_Width = {};

        SPARK_MEMBER(Serialized)
        uint32_t m_Height = {};

        SPARK_MEMBER(Serialized)
        uint32_t m_MipCount = {};

        SPARK_MEMBER(Serialized)
        std::vector<texture::Mipmap> m_Mips = {};

        SPARK_MEMBER(Serialized)
        texture::Format m_Format = {};

        SPARK_MEMBER(Serialized)
        texture::Usage m_Usage = texture::Usage::Read;

        std::shared_ptr<render::TextureResource> m_PlatformResource = nullptr;
    };
}
