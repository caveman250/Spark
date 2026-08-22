module;

#include "spark.h"

export module Spark.Render.IndexBuffer;
import Spark.Asset.Mesh.StaticMesh;

namespace se::render
{
    export class IndexBuffer
    {
    public:
        static std::shared_ptr<IndexBuffer> CreateIndexBuffer(const asset::StaticMesh& mesh);

        IndexBuffer(const asset::StaticMesh& mesh);
        virtual ~IndexBuffer() = default;
        virtual void CreatePlatformResource() = 0;
        virtual void Bind() = 0;

        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

    protected:
        std::vector<uint32_t> m_Indices;
    };
}
