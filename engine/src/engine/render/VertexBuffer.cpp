#include "VertexBuffer.h"

#include "engine/asset/mesh/StaticMesh.h"

namespace se::render
{
    size_t VertexBuffer::GetVertexCount() const
    {
        if (m_VertexStreams.empty())
        {
            return 0;
        }

        auto& firstStream = m_VertexStreams.begin()->second;
        return firstStream.data.size() / firstStream.stride;

    }

    VertexBuffer::VertexBuffer(const asset::StaticMesh& mesh)
    {
        GenerateVertexStreams(mesh);
    }

    VertexBuffer::VertexBuffer(const std::vector<debug::Line>& lines)
    {
        if (!lines.empty())
        {
            VertexStream& pos = m_VertexStreams[VertexStreamType::Position];
            pos.type = VertexStreamType::Position;
            pos.stride = 3;
            pos.data.reserve(lines.size() * 3 * 2);
            VertexStream& colour = m_VertexStreams[VertexStreamType::Colour];
            colour.type = VertexStreamType::Colour;
            colour.stride = 4;
            colour.data.reserve(lines.size() * 4 * 2);
            for (const auto& line : lines)
            {
                pos.data.push_back(line.start.x);
                pos.data.push_back(line.start.y);
                pos.data.push_back(line.start.z);
                colour.data.push_back(line.colour.x);
                colour.data.push_back(line.colour.y);
                colour.data.push_back(line.colour.z);
                colour.data.push_back(line.colour.w);

                pos.data.push_back(line.end.x);
                pos.data.push_back(line.end.y);
                pos.data.push_back(line.end.z);
                colour.data.push_back(line.colour.x);
                colour.data.push_back(line.colour.y);
                colour.data.push_back(line.colour.z);
                colour.data.push_back(line.colour.w);
            }
        }
    }

    void VertexBuffer::GenerateVertexStreams(const asset::StaticMesh& mesh)
    {
        if (!mesh.vertices.empty())
        {
            VertexStream& stream = m_VertexStreams[VertexStreamType::Position];
            stream.type = VertexStreamType::Position;
            stream.stride = 3;
            stream.data.reserve(mesh.vertices.size() * 3);
            for (const auto& vec : mesh.vertices)
            {
                stream.data.push_back(vec.x);
                stream.data.push_back(vec.y);
                stream.data.push_back(vec.z);
            }
        }

        if (!mesh.normals.empty())
        {
            VertexStream& stream = m_VertexStreams[VertexStreamType::Normal];
            stream.type = VertexStreamType::Normal;
            stream.stride = 3;
            stream.data.reserve(mesh.normals.size() * 3);
            for (const auto& vec : mesh.normals)
            {
                stream.data.push_back(vec.x);
                stream.data.push_back(vec.y);
                stream.data.push_back(vec.z);
            }
        }

        if (!mesh.uvs.empty())
        {
            VertexStream& stream = m_VertexStreams[VertexStreamType::UV];
            stream.type = VertexStreamType::UV;
            stream.stride = 2;
            stream.data.reserve(mesh.uvs.size() * 3);
            for (const auto& vec : mesh.uvs)
            {
                stream.data.push_back(vec.x);
                stream.data.push_back(vec.y);
            }
        }
    }
}
