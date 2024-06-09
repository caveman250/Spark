#include "engine/render/RenderCommand.h"
#include "spark.h"
#include "GL_fwd.h"
#include "engine/render/Material.h"
#include "engine/render/VertexBuffer.h"


namespace se::render::commands
{
    void Clear::Execute()
    {
        GLbitfield mask = 0;
        if (m_ClearColour)
        {
            mask |= GL_COLOR_BUFFER_BIT;
        }
        if (m_ClearDepth)
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        glClear(mask);
    }

    void SubmitGeo::Execute()
    {
        m_Material->Bind();
        m_VertBuffer->Bind();

        glDrawArrays(GL_TRIANGLES, 0, m_IndexCount);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
        GL_CHECK_ERROR()
    }

    Clear::Clear(bool clearColour, bool clearDepth)
        : m_ClearColour(clearColour)
        , m_ClearDepth(clearDepth)
    {

    }

    SubmitGeo::SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, int indexCount)
        : m_Material(material)
        , m_VertBuffer(vertBuffer)
        , m_IndexCount(indexCount)
    {

    }
}
