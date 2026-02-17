#include "IndexBuffer.h"

#include "engine/threads/util/ThreadUtil.h"
#include "DeferredOpenGLOperations.h"

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
        if (threads::util::IsMainThread())
        {
            EASY_FUNCTION();
            glGenBuffers(1, &m_Resource);
            GL_CHECK_ERROR()
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Resource);
            GL_CHECK_ERROR()
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), &m_Indices[0], GL_STATIC_DRAW);
            GL_CHECK_ERROR()
        }
        else
        {
            DeferredOpenGLOperations::Get()->AddDeferredOp([this]()
            {
                CreatePlatformResource();
            });
        }
    }

    void IndexBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Resource);
        GL_CHECK_ERROR()
    }

    void IndexBuffer::Cleanup()
    {
        if (threads::util::IsMainThread())
        {
            Cleanup(m_Resource);
        }
        else
        {
            DeferredOpenGLOperations::Get()->AddDeferredOp([this]()
            {
                Cleanup(m_Resource);
            });
        }
    }

    void IndexBuffer::Cleanup(GLuint resource)
    {
        if (resource != GL_INVALID_VALUE)
        {
            SPARK_ASSERT(resource != 1);
            glDeleteBuffers(1, &resource);
            GL_CHECK_ERROR()
        }
    }
}

#endif
