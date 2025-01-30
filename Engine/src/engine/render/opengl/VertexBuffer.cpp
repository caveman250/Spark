#include "VertexBuffer.h"

#include "engine/math/Vec3.h"

namespace se::render
{
    std::shared_ptr<VertexBuffer> VertexBuffer::CreateVertexBuffer(const asset::StaticMesh& mesh)
    {
        return std::make_shared<opengl::VertexBuffer>(mesh);
    }
}

namespace se::render::opengl
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
        Cleanup();

        glGenVertexArrays(1, &m_VertexArrayID);
        GL_CHECK_ERROR()
        glBindVertexArray(m_VertexArrayID);
        GL_CHECK_ERROR()

        for (const auto& [usage, stream] : m_VertexStreams)
        {
            glGenBuffers(1, &m_GlResources[usage]);
            GL_CHECK_ERROR()
            glBindBuffer(GL_ARRAY_BUFFER, m_GlResources[usage]);
            GL_CHECK_ERROR()
            glBufferData(GL_ARRAY_BUFFER, stream.data.size() * sizeof(float), stream.data.data(), GL_STATIC_DRAW);
            GL_CHECK_ERROR()
        }
    }

    void VertexBuffer::Bind()
    {
        glBindVertexArray(m_VertexArrayID);
        GL_CHECK_ERROR()

        uint8_t loc = 0;
        for (const auto& [usage, stream] : m_VertexStreams)
        {
            auto buffer = m_GlResources[usage];
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            GL_CHECK_ERROR()
            glEnableVertexAttribArray(loc);
            GL_CHECK_ERROR()
            glVertexAttribPointer(
               loc,
               stream.stride,
               GL_FLOAT,
               GL_FALSE,
               0,
               nullptr
            );
            GL_CHECK_ERROR()
            loc++;
        }
    }

    void VertexBuffer::Unbind()
    {
        for (size_t loc = 0; loc < m_VertexStreams.size(); ++loc)
        {
            glDisableVertexAttribArray(static_cast<int>(loc++));
            GL_CHECK_ERROR()
        }
    }

    void VertexBuffer::Cleanup()
    {
        if (m_VertexArrayID != GL_INVALID_VALUE)
        {
            glDeleteVertexArrays(1, &m_VertexArrayID);
            m_VertexArrayID = GL_INVALID_VALUE;
            GL_CHECK_ERROR()
        }
        for (const auto& [usage, stream] : m_VertexStreams)
        {
            SPARK_ASSERT(m_GlResources[usage] != 1);
            glDeleteBuffers(1, &m_GlResources[usage]);
            m_GlResources[usage] = GL_INVALID_VALUE;
            GL_CHECK_ERROR()
        }
    }
}
