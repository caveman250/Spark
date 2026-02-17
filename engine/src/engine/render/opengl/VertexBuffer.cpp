#include "VertexBuffer.h"
#if OPENGL_RENDERER
#include "DeferredOpenGLOperations.h"
#include "engine/math/Vec3.h"
#include "engine/threads/util/ThreadUtil.h"

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
        if (threads::util::IsMainThread())
        {
            EASY_FUNCTION();
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
        else
        {
            DeferredOpenGLOperations::Get()->AddDeferredOp([this]()
            {
                CreatePlatformResource();
            });
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
        std::vector<GLuint> resources = {};
        resources.reserve(m_GlResources.size());
        for (const auto& resource : m_GlResources | std::views::values)
        {
            resources.push_back(resource);
        }

        if (threads::util::IsMainThread())
        {
            Cleanup(m_VertexArrayID, resources);
            m_VertexArrayID = GL_INVALID_VALUE;
        }
        else
        {
            DeferredOpenGLOperations::Get()->AddDeferredOp([this, resources]()
            {
                Cleanup(m_VertexArrayID, resources);
            });
        }
    }

    void VertexBuffer::Cleanup(GLuint vertexArrayId,
        const std::vector<GLuint>& resources)
    {
        if (vertexArrayId != GL_INVALID_VALUE)
        {
            glDeleteVertexArrays(1, &vertexArrayId);
            GL_CHECK_ERROR()
        }
        for (const auto& resource : resources)
        {
            glDeleteBuffers(1, &resource);
            GL_CHECK_ERROR()
        }
    }
}
#endif
