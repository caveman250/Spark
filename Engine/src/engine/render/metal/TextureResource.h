#pragma once

#include "engine/render/render_fwd.h"

#if METAL_RENDERER
#include "engine/render/metal/MTL_fwd.h"
#include "engine/render/TextureResource.h"

#if __OBJC__
@protocol MTLTexture;
typedef id<MTLTexture> MTLTexturePtr;
#else
typedef void* MTLBufferPtr;
#endif

namespace se::render::metal
{
    class TextureResource : public render::TextureResource
    {
    public:
        TextureResource(const asset::Texture& texture) : render::TextureResource(texture) {}
        void CreatePlatformResources() override;
        void Bind(size_t i) override;

    private:

        MTLTexturePtr m_MetalResource;
    };
}

#endif
