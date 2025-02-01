#include "IndexBuffer.h"

#if METAL_RENDERER

namespace se::render
{
    std::shared_ptr<IndexBuffer> IndexBuffer::CreateIndexBuffer(const asset::StaticMesh& mesh)
    {
        return std::make_shared<metal::IndexBuffer>(mesh);
    }

}
namespace se::render::metal
{
    IndexBuffer::IndexBuffer(const asset::StaticMesh& mesh)
        : render::IndexBuffer(mesh)
    {
    }

    IndexBuffer::~IndexBuffer()
    {
    }

    void IndexBuffer::CreatePlatformResource()
    {
        // TODO
    }

    void IndexBuffer::Bind()
    {
        // TODO
    }
}

#endif
