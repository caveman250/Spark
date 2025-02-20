#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"

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

        bool HasCreatedPlatformResources() const { return m_HasCreatedPlatformResources; }
    protected:
        TextureResource(const asset::Texture& texture) : m_Texture(texture) {}
        const asset::Texture& m_Texture;
        bool m_HasCreatedPlatformResources = false;
    };
}