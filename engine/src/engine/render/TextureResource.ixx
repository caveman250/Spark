module;

#include "spark.h"

export module Spark.Render.TextureResource;
import Spark.Asset.Asset;
import Spark.Asset.Texture.Format;
import Spark.Asset.Texture.Mipmap;

namespace se::render
{
    export class TextureResource
    {
    public:
        virtual ~TextureResource() = default;

        virtual void CreatePlatformResources()
        {
            m_HasCreatedPlatformResources = true;
        }

        virtual void Bind(size_t i) = 0;

        bool HasCreatedPlatformResources() const
        {
            return m_HasCreatedPlatformResources;
        }

    protected:
        TextureResource(size_t width,
            size_t height,
            asset::texture::Format format,
            asset::texture::Usage usage,
            const std::vector<asset::texture::Mipmap>& mips);

        size_t m_Width;
        size_t m_Height;
        asset::texture::Format m_Format;
        asset::texture::Usage m_Usage;
        std::vector<asset::texture::Mipmap> m_MipData;
        bool m_HasCreatedPlatformResources = false;
    };
}
