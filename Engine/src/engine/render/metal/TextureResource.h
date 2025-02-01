#pragma once

#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/render/TextureResource.h"

namespace se::render::metal
{
    class TextureResource : public render::TextureResource
    {
    public:
        TextureResource(const asset::Texture& texture) : render::TextureResource(texture) {}
        void CreatePlatformResources() override;
        void Bind() override;
    };
}

#endif
