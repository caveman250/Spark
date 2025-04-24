#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/math/math.h"

namespace se::render
{
    class TextureResource
    {
    public:
        static std::shared_ptr<TextureResource> Create(const asset::Texture& texture);
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
        TextureResource(const asset::Texture& texture);
        size_t m_Width;
        size_t m_Height;
        asset::texture::Format::Type m_Format;
        asset::texture::Usage::Type m_Usage;
        std::vector<asset::texture::Mipmap> m_MipData;
        bool m_HasCreatedPlatformResources = false;
    };
}
