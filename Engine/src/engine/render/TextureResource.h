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
        virtual void CreatePlatformResources() = 0;
        virtual void Bind() = 0;

    protected:
        TextureResource(const asset::Texture& texture) : m_Texture(texture) {}
        const asset::Texture& m_Texture;
    };
}