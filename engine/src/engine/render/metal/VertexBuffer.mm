#import "VertexBuffer.h"
#import "engine/render/metal/MetalRenderer.h"
#import <Metal/Metal.h>
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
        auto device = se::render::Renderer::Get<se::render::metal::MetalRenderer>()->GetDevice();

        for (const auto& [usage, stream]: m_VertexStreams)
        {
            auto stride = stream.stride;
            auto streamStride = GetStreamStride(usage);
            auto numElements = stream.data.size();

            size_t bufferSize = numElements * sizeof(float) * streamStride;
            auto buffer= m_Buffers.emplace_back([device newBufferWithLength:bufferSize options:MTLStorageModeShared]);
            [buffer retain];
            for (size_t i = 0; i < numElements / stride; ++i)
            {
                float* dest = static_cast<float*>([buffer contents]) + i * streamStride;
                float* source = const_cast<float*>(stream.data.data()) + i * stride;
                memcpy(dest, source, stride * sizeof(float));
                if (stride < streamStride)
                {
                    float* paddingDest = dest + stride;
                    memset(paddingDest, 0, (streamStride - stride) * sizeof(float));
                }
            }
        }
    }

    void VertexBuffer::Bind()
    {
        auto renderer = se::render::Renderer::Get<se::render::metal::MetalRenderer>();
        for (size_t i = 0; i < m_VertexStreams.size(); ++i)
        {
            [renderer->GetCurrentCommandEncoder() setVertexBuffer:m_Buffers[i] offset:0 atIndex:i];
        }
    }

    void VertexBuffer::Unbind()
    {
        // nothing to do?
    }

    void VertexBuffer::Cleanup()
    {
        for (const auto& buffer : m_Buffers)
        {
            [buffer release];
        }
        m_Buffers.clear();
    }

    size_t VertexBuffer::GetStreamStride(se::render::VertexStreamType streamType)
    {
        switch (streamType)
        {
            case se::render::VertexStreamType::UV:
                return 2;
            case se::render::VertexStreamType::Position:
            case se::render::VertexStreamType::Colour:
            case se::render::VertexStreamType::Normal:
            case se::render::VertexStreamType::Tangent:
            case se::render::VertexStreamType::Bitangent:
                return 4;
            default:
                SPARK_ASSERT(false, "VertexBuffer::GetStreamStride - Unhandled VertexStreamType");
                return 0;
        }
    }
}
#endif
