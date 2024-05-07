#pragma once
#include "engine/asset/AssetBase.h"

namespace se::asset::builder
{
    struct CompressedImageData;
}

namespace se::asset
{
    namespace texture
    {
        enum class Format;
        struct Mipmap;
    }

    class Texture : public AssetBase
    {
    public:
        ~Texture();
        std::shared_ptr<binary::Database> Serialise() override;
        void Deserialise(const std::shared_ptr<binary::Database>& db) override;
        static Texture FromDDS(const builder::CompressedImageData& rawDDSData);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetMipCount() const { return m_MipCount; }
        const std::vector<texture::Mipmap>& GetMips() const { return m_Mips; }
        texture::Format GetFormat() const { return m_Format; }
    private:
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_MipCount;
        std::vector<texture::Mipmap> m_Mips;
        texture::Format m_Format;
    };
}
