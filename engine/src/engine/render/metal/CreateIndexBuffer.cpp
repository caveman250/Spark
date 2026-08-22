module;

#include <memory>

#if METAL_RENDERER

module Spark.Render.IndexBuffer;
import Spark.Render.Metal.IndexBuffer;

namespace se::render
{
    std::shared_ptr<IndexBuffer> IndexBuffer::CreateIndexBuffer(const asset::StaticMesh& mesh)
    {
        return std::make_shared<metal::IndexBuffer>(mesh);
    }

}

#endif