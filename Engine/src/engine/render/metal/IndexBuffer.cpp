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
        auto device = Renderer::Get<MetalRenderer>()->GetDevice();
        size_t dataSize = m_Indices.size() * sizeof(uint32_t);

        m_Buffer = device->newBuffer(dataSize, MTL::ResourceStorageModeManaged);
        memcpy(m_Buffer->contents(), m_Indices.data(), dataSize);
        m_Buffer->didModifyRange(NS::Range::Make(0, m_Buffer->length()));
    }

    void IndexBuffer::Bind()
    {
        // not relevant
    }

    void IndexBuffer::Cleanup()
    {
        m_Buffer->release();
    }
}

#endif
