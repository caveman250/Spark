#pragma once

#include "spark.h"
#include "engine/render/metal/MTL_fwd.h"
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/render/metal/MTL_fwd.h"

namespace se::render::metal::wrappers
{
    class TextureResource
    {
    public:
        static MTLTexturePtr CreateMetalResource(MTLDevicePtr device,
            size_t width,
            size_t height,
            MTLPixelFormat pixelFormat,
            MTLTextureUsage usage);

        static void ReplaceRegon(MTLTexturePtr texture,
            MTLRegion region,
            NSUInteger mipmapLevel,
            const void* withBytes,
            NSUInteger bytesPerRow);

        static void Bind(MTLRenderCommandEncoderPtr commandEncoder, MTLTexturePtr texture, size_t i);
    };
}

#endif
