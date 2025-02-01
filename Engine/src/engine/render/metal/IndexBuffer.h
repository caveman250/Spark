#pragma once

#include "engine/render/render_fwd.h"

#if METAL_RENDERER

#include "spark.h"
#include "engine/render/IndexBuffer.h"

namespace se::render::metal
{
    class IndexBuffer : public render::IndexBuffer
    {
    public:
        IndexBuffer(const asset::StaticMesh& mesh);
        ~IndexBuffer();

        void CreatePlatformResource() override;
        void Bind() override;
    };
}

#endif
