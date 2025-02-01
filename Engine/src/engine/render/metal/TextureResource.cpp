#include "TextureResource.h"

#if METAL_RENDERER

namespace se::render
{
    std::shared_ptr<TextureResource> TextureResource::Create(const asset::Texture &texture)
    {
        return std::make_shared<metal::TextureResource>(texture);
    }
}

namespace se::render::metal
{
    void TextureResource::CreatePlatformResources()
    {
        render::TextureResource::CreatePlatformResources();
        // TODO
    }

    void TextureResource::Bind()
    {
        // TODO
    }
}
#endif
