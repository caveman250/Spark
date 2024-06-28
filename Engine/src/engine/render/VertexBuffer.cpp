#include "VertexBuffer.h"

#include "engine/asset/mesh/StaticMesh.h"

namespace se::render
{
    VertexBuffer::VertexBuffer(const asset::StaticMesh& mesh)
    {
        GenerateVertexStreams(mesh);
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
