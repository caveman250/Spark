#include "VertexBuffer.h"

#include <Gl/glew.h>

#include "engine/math/Vec3.h"

namespace se::render
{
    std::shared_ptr<VertexBuffer> VertexBuffer::CreateVertexBuffer(const std::vector<math::Vec3> &vertices)
    {
        return std::make_shared<opengl::VertexBuffer>(vertices);
    }
}

namespace se::render::opengl
{
    void VertexBuffer::CreatePlatformResource()
    {
        glGenBuffers(1, &m_GlResource);
        glBindBuffer(GL_ARRAY_BUFFER, m_GlResource);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(math::Vec3), m_Vertices.data(), GL_STATIC_DRAW);
    }

    void VertexBuffer::Bind()
    {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_GlResource);
        glVertexAttribPointer(
           0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
           3,                  // size
           GL_FLOAT,           // type
           GL_FALSE,           // normalized?
           0,                  // stride
           (void*)0            // array buffer offset
        );
    }

    void VertexBuffer::Unbind()
    {
        glDisableVertexAttribArray(0);
    }

    VertexBuffer::VertexBuffer(const std::vector<math::Vec3> &vertices)
        : render::VertexBuffer(vertices)
    {

    }
}
