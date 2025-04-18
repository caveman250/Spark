#include "spark.h"
#if OPENGL_RENDERER
#include "engine/render/RenderCommand.h"
#include "engine/Application.h"
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
        GL_CHECK_ERROR()
    }

    void SubmitGeo::Execute()
    {
        m_MaterialInstance->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();
        m_IndexBuffer->Bind();

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexBuffer->GetIndices().size()), GL_UNSIGNED_INT, (void*)0);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
    }

    void SubmitUI::Execute()
    {
        m_MaterialInstance->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();
        m_IndexBuffer->Bind();

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexBuffer->GetIndices().size()), GL_UNSIGNED_INT, (void*)0);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
    }

    void PushScissor::Execute()
    {
        auto primaryWindow = Application::Get()->GetPrimaryWindow();
        glEnable(GL_SCISSOR_TEST);
        GL_CHECK_ERROR()
        glScissor(m_Rect.topLeft.x,
                  primaryWindow->GetHeight() - (m_Rect.topLeft.y + m_Rect.size.y),
                  m_Rect.size.x,
                  m_Rect.size.y);
        GL_CHECK_ERROR()

    }

    void PopScissor::Execute()
    {
        glDisable(GL_SCISSOR_TEST); // TODO keep track of stack
        GL_CHECK_ERROR()
    }
}

#endif
