#include "VertexBuffer.h"

#include "engine/math/Vec3.h"

namespace se::render
{
    std::shared_ptr<VertexBuffer> VertexBuffer::CreateVertexBuffer(const std::vector<VertexStream>& streams)
    {
        return std::make_shared<opengl::VertexBuffer>(streams);
    }
}

namespace se::render::opengl
{
    void VertexBuffer::CreatePlatformResource()
    {
        GLuint VertexArrayID = {};
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        for (const auto& [usage, stream] : m_VertexStreams)
        {
            glGenBuffers(1, &m_GlResources[usage]);
            glBindBuffer(GL_ARRAY_BUFFER, m_GlResources[usage]);
            glBufferData(GL_ARRAY_BUFFER, stream.data.size() * sizeof(float), stream.data.data(), GL_STATIC_DRAW);
        }
    }

    void VertexBuffer::Bind()
    {
        uint8_t loc = 0;
        for (const auto& [usage, stream] : m_VertexStreams)
        {
            glEnableVertexAttribArray(loc);
            glBindBuffer(GL_ARRAY_BUFFER, m_GlResources[usage]);
            glVertexAttribPointer(
               loc,
               stream.stride,
               GL_FLOAT,
               GL_FALSE,
               0,
               nullptr
            );
            loc++;
        }
    }

    void VertexBuffer::Unbind()
    {
        uint8_t loc = 0;
        for (const auto& [usage, stream] : m_VertexStreams)
        {
            glDisableVertexAttribArray(loc++);
        }
    }

    VertexBuffer::VertexBuffer(const std::vector<VertexStream>& streams)
        : render::VertexBuffer(streams)
    {

    }
}
