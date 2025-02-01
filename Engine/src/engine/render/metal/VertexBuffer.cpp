#include "VertexBuffer.h"
#if METAL_RENDERER
#include "engine/math/Vec3.h"

namespace se::render
{
    std::shared_ptr<VertexBuffer> VertexBuffer::CreateVertexBuffer(const asset::StaticMesh& mesh)
    {
        return std::make_shared<metal::VertexBuffer>(mesh);
    }
}

namespace se::render::metal
{
    VertexBuffer::VertexBuffer(const asset::StaticMesh& mesh)
    : render::VertexBuffer(mesh)
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        Cleanup();
    }

    void VertexBuffer::CreatePlatformResource()
    {
        // TODO
    }

    void VertexBuffer::Bind()
    {
        // TODO
    }

    void VertexBuffer::Unbind()
    {
        // TODO
    }

    void VertexBuffer::Cleanup()
    {
        // TODO
    }
}
#endif
