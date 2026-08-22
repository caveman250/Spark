module;
#include "engine/render/render_fwd.h"

#if METAL_RENDERER
#include "engine/render/metal/MTL_fwd.h"

export module Spark.Render.Metal.VertexBuffer;
import Spark.Render.VertexBuffer;

namespace se::render::metal
{
    export class VertexBuffer : public render::VertexBuffer
    {
    public:
        explicit VertexBuffer(const asset::StaticMesh& mesh);
        explicit VertexBuffer(const std::vector<debug::Line>& lines);
        ~VertexBuffer() override;

        void CreatePlatformResource() override;
        void Bind() override;
        void Unbind() override;

        MTLBufferPtr GetMTLBuffer(size_t index) { return m_Buffers[index]; }
    private:
        void Cleanup();

        static size_t GetStreamStride(VertexStreamType streamType);

        std::vector<MTLBufferPtr> m_Buffers;
    };
}
#endif
