module;

#include "MTL_fwd.h"
#include "wrappers/IndexBuffer.h"

#if METAL_RENDERER

module Spark.Render.Metal.IndexBuffer;
import Spark.Render.Renderer;
import Spark.Render.Metal.MetalRenderer;

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
        wrappers::IndexBuffer::CreatePlatfromResources(device, dataSize, m_Indices.data());
    }

    void IndexBuffer::Bind()
    {
        // not relevant
    }

    void IndexBuffer::Cleanup()
    {
        wrappers::IndexBuffer::Release(m_Buffer);
    }
}

#endif
