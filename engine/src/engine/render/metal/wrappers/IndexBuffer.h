#pragma once

#include "spark.h"
#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/render/metal/MTL_fwd.h"

namespace se::render::metal::wrappers
{
    class IndexBuffer
    {
    public:
        static MTLBufferPtr CreatePlatfromResources(MTLDevicePtr device, size_t size, uint32_t* data);
        static void Release(MTLBufferPtr buffer);
    };
}

#endif