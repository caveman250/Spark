#include "IndexBuffer.h"

#if OPENGL_RENDERER

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

    IndexBuffer::~IndexBuffer()
    {
        Cleanup();
    }

    void IndexBuffer::CreatePlatformResource()
    {
        glGenBuffers(1, &m_Resource);
        GL_CHECK_ERROR()
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Resource);
        GL_CHECK_ERROR()
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), &m_Indices[0], GL_STATIC_DRAW);
        GL_CHECK_ERROR()
    }

    void IndexBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Resource);
        GL_CHECK_ERROR()
    }

    void IndexBuffer::Cleanup()
    {
        if (m_Resource != GL_INVALID_VALUE)
        {
            SPARK_ASSERT(m_Resource != 1);
            glDeleteBuffers(1, &m_Resource);
            GL_CHECK_ERROR()
        }
    }
}

#endif
