module;

#include "spark.h"
#include <map>

export module Spark.Render.VertexBuffer;
import Spark.Asset.Mesh.StaticMesh;
import Spark.Math;
import Spark.Debug.Graphics;

namespace se::render
{
    export enum class VertexStreamType
    {
        Position,
        Colour,
        UV,
        Normal,
        Tangent,
        Bitangent
    };

    export struct VertexStream
    {
        VertexStreamType type;
        uint8_t stride;
        std::vector<float> data;
    };

    export class VertexBuffer
    {
    public:
        static std::shared_ptr<VertexBuffer> CreateVertexBuffer(const asset::StaticMesh& mesh);
        static std::shared_ptr<VertexBuffer> CreateVertexBuffer(const std::vector<debug::Line>& lines);

        size_t GetVertexCount() const;

        VertexBuffer(const asset::StaticMesh& mesh);
        VertexBuffer(const std::vector<debug::Line>& lines);
        virtual ~VertexBuffer() = default;
        virtual void CreatePlatformResource() = 0;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        const std::map<VertexStreamType, VertexStream>& GetVertexStreams() const { return m_VertexStreams; }

    protected:
        void GenerateVertexStreams(const asset::StaticMesh& mesh);

        std::map<VertexStreamType, VertexStream> m_VertexStreams;
    };
}
