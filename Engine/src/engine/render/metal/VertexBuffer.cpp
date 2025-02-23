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

    size_t VertexBuffer::GetStreamStride(VertexStreamType streamType)
    {
        switch (streamType)
        {
            case VertexStreamType::UV:
                return 2;
            case VertexStreamType::Position:
            case VertexStreamType::Colour:
            case VertexStreamType::Normal:
            case VertexStreamType::Tangent:
            case VertexStreamType::Bitangent:
                return 4;
            default:
                SPARK_ASSERT(false, "VertexBuffer::GetStreamStride - Unhandled VertexStreamType");
                return 0;
        }
    }

    void VertexBuffer::CreatePlatformResource()
    {
        auto device = Renderer::Get<MetalRenderer>()->GetDevice();

        for (const auto& [usage, stream]: m_VertexStreams)
        {
            auto stride = stream.stride;
            auto streamStride = GetStreamStride(usage);
            auto numElements = stream.data.size();
            
            size_t bufferSize = numElements * sizeof(float) * streamStride;
            auto& buffer = m_Buffers.emplace_back(device->newBuffer(bufferSize, MTL::ResourceStorageModeManaged));
            
            for (int i = 0; i < numElements; ++i)
            {
                float* dest = static_cast<float*>(buffer->contents()) + i * streamStride;
                float* source = const_cast<float*>(stream.data.data()) + i * stride;
                memcpy(dest, source, stride * sizeof(float));
                if (stride < streamStride)
                {
                    float* paddingDest = dest + stride;
                    memset(paddingDest, 0, (streamStride - stride) * sizeof(float));
                }
            }
            
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
