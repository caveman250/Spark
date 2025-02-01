#include "VertexBuffer.h"

#include "MetalRenderer.h"
#if METAL_RENDERER

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
        auto device = Renderer::Get<MetalRenderer>()->GetDevice();

        for (const auto& [usage, stream]: m_VertexStreams)
        {
            size_t dataSize = stream.data.size() * sizeof(float);
            auto& buffer = m_Buffers.emplace_back(device->newBuffer(dataSize, MTL::ResourceStorageModeManaged));
            memcpy(buffer->contents(), stream.data.data(), dataSize);
            buffer->didModifyRange(NS::Range::Make(0, buffer->length()));
        }
    }

    void VertexBuffer::Bind()
    {
        auto commandEncoder = Renderer::Get<MetalRenderer>()->GetCurrentCommandEncoder();
        for (size_t i = 0; i < m_VertexStreams.size(); ++i)
        {
            commandEncoder->setVertexBuffer( m_Buffers[i], 0, i );
        }
    }

    void VertexBuffer::Unbind()
    {
        // nothing to do?
    }

    void VertexBuffer::Cleanup()
    {
        for (auto buffer : m_Buffers)
        {
            buffer->release();
        }
        m_Buffers.clear();
    }
}
#endif
