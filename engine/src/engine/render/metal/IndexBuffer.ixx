module;

#include "engine/render/render_fwd.h"
#include "engine/render/metal/MTL_fwd.h"

#if METAL_RENDERER

export module Spark.Render.Metal.IndexBuffer;
import Spark.Render.IndexBuffer;
import Spark.Asset.Mesh.StaticMesh;

namespace se::render::metal
{
    export class IndexBuffer : public render::IndexBuffer
    {
    public:
        IndexBuffer(const asset::StaticMesh& mesh);
        ~IndexBuffer();

        void CreatePlatformResource() override;
        void Bind() override;

        MTLBufferPtr GetMTLBuffer() const { return m_Buffer; }
    private:
        void Cleanup();

        MTLBufferPtr m_Buffer;
    };
}

#endif
