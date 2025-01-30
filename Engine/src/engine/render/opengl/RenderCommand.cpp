#include "spark.h"
#include "engine/render/RenderCommand.h"
#include "engine/Application.h"
#include "GL_fwd.h"
#include "engine/render/Material.h"
#include "engine/render/VertexBuffer.h"

DEFINE_SPARK_ENUM_BEGIN(se::render::commands::RenderStage)
    DEFINE_ENUM_VALUE(RenderStage, Scene)
    DEFINE_ENUM_VALUE(RenderStage, UI)
DEFINE_SPARK_ENUM_END()

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
        GL_CHECK_ERROR()
    }

    void SubmitGeo::Execute()
    {
        m_Material->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();
        m_IndexBuffer->Bind();

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexBuffer->GetIndices().size()), GL_UNSIGNED_INT, (void*)0);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
    }

    SubmitUI::SubmitUI(const std::shared_ptr<Material> &material, const std::shared_ptr<VertexBuffer> &vertBuffer,
        const std::shared_ptr<IndexBuffer> &indexBuffer)
        : m_Material(material)
        , m_VertBuffer(vertBuffer)
        , m_IndexBuffer(indexBuffer)
    {

    }

    void SubmitUI::Execute()
    {
        m_Material->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();
        m_IndexBuffer->Bind();

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexBuffer->GetIndices().size()), GL_UNSIGNED_INT, (void*)0);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
    }

    Clear::Clear(bool clearColour, bool clearDepth)
        : m_ClearColour(clearColour)
        , m_ClearDepth(clearDepth)
    {

    }

    SubmitGeo::SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
        : m_Material(material)
        , m_VertBuffer(vertBuffer)
        , m_IndexBuffer(indexBuffer)
    {

    }

    PushScissor::PushScissor(const ui::Rect& rect)
            : m_Rect(rect)
    {

    }

    void PushScissor::Execute()
    {
        auto primaryWindow = Application::Get()->GetPrimaryWindow(); // TODO
        glEnable(GL_SCISSOR_TEST);
        GL_CHECK_ERROR()
        glScissor(m_Rect.topLeft.x,
                  primaryWindow->GetHeight() - (m_Rect.topLeft.y + m_Rect.size.y),
                  m_Rect.size.x,
                  m_Rect.size.y);
        GL_CHECK_ERROR()

    }

    PopScissor::PopScissor()
    {

    }

    void PopScissor::Execute()
    {
        glDisable(GL_SCISSOR_TEST); // TODO keep track of stack
        GL_CHECK_ERROR()
    }
}
