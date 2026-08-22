#pragma once

#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "engine/render/metal/MTL_fwd.h"

namespace se::render::metal::wrappers
{
    class RenderCommand
    {
    public:
        static void RenderGeo(MTLRenderCommandEncoderPtr commandEncoder,
            MTLPrimitiveType primType,
            MTLBufferPtr vertexBuffer,
            size_t numVertices);

        static void RenderIndexedGeo(MTLRenderCommandEncoderPtr commandEncoder,
            MTLPrimitiveType primType,
            MTLBufferPtr indexBuffer,
            size_t numIndices);

        static void SetScissorRect(MTLRenderCommandEncoderPtr commandEncoder, MTLScissorRect scissor);
    };
}

#endif
