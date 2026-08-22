module;

#include "spark.h"

module Spark.Render.IndexBuffer;

namespace se::render
{
    IndexBuffer::IndexBuffer(const asset::StaticMesh& mesh)
    {
        m_Indices.reserve(mesh.indices.size());
        for (const auto& index : mesh.indices)
        {
            m_Indices.push_back(index);
        }
    }
}
