#include "IndexBuffer.h"

namespace se::render
{
    std::shared_ptr<IndexBuffer> IndexBuffer::CreateIndexBuffer(const asset::StaticMesh& mesh)
    {
        return std::make_shared<opengl::IndexBuffer>(mesh);
    }

}
namespace se::render::opengl
{
    IndexBuffer::IndexBuffer(const asset::StaticMesh& mesh)
        : render::IndexBuffer(mesh)
    {
    }

    void IndexBuffer::CreatePlatformResource()
    {
        glGenBuffers(1, &m_Resource);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Resource);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), &m_Indices[0], GL_STATIC_DRAW);
    }

    void IndexBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Resource);
    }
}
