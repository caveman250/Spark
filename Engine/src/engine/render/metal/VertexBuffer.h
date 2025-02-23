#pragma once
#include "engine/render/render_fwd.h"

#if METAL_RENDERER
#include "engine/render/metal/MTL_fwd.h"
#include "engine/render/VertexBuffer.h"

namespace se::render::metal
{
    class VertexBuffer : public render::VertexBuffer
    {
    public:
        VertexBuffer(const asset::StaticMesh& mesh);
        ~VertexBuffer();

        void CreatePlatformResource() override;
        void Bind() override;
        void Unbind() override;
    private:
        void Cleanup();
        size_t GetStreamStride(VertexStreamType streamType);

        std::vector<MTL::Buffer*> m_Buffers;
    };
}
#endif
