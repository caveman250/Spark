module;

#include "spark.h"
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

export module Spark.Render.Metal.TextureResource;
import Spark.Render.TextureResource;
import Spark.Asset.Texture.Format;
import Spark.Asset.Texture.Mipmap;

namespace se::render::metal
{
    export class TextureResource : public render::TextureResource
    {
    public:
        TextureResource(size_t width,
            size_t height,
            asset::texture::Format format,
            asset::texture::Usage usage,
            const std::vector<asset::texture::Mipmap>& mips)
            : render::TextureResource(width, height, format, usage, mips)
        {
        }

        void CreatePlatformResources() override;
        void Bind(size_t i) override;
        MTLTexturePtr GetMetalResource() const { return m_MetalResource; }

    private:
        MTLTexturePtr m_MetalResource = nullptr;
    };
}

#endif
