#if OPENGL_RENDERER
#include "engine/render/RenderCommand.h"
#include "engine/Application.h"
#include "GL_fwd.h"
#include "engine/render/Material.h"
#include "engine/render/VertexBuffer.h"
#include "platform/IWindow.h"

namespace se::render::commands
{
    void Clear::Execute()
    {
        EASY_BLOCK("Clear");
        GLbitfield mask = 0;
        if (m_ClearColour)
        {
            mask |= GL_COLOR_BUFFER_BIT;
        }
        if (m_ClearDepth)
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        GL_CHECK_ERROR()
        glClear(mask);
        GL_CHECK_ERROR()
    }

    void SubmitGeo::Execute()
    {
        EASY_BLOCK("SubmitGeo");
        m_MaterialInstance->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();
        m_IndexBuffer->Bind();

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexBuffer->GetIndices().size()), GL_UNSIGNED_INT, (void*)0);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
    }

    void SubmitUI::Execute()
    {
        EASY_BLOCK("SubmitUI");
        m_MaterialInstance->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();
        m_IndexBuffer->Bind();

        glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndexBuffer->GetIndices().size()), GL_UNSIGNED_INT, (void*)0);
        GL_CHECK_ERROR()

        m_VertBuffer->Unbind();
    }

    void PushScissor::Execute()
    {
        EASY_BLOCK("PushScissor");
        if (m_Rect.size.x <= 0 || m_Rect.size.y <= 0)
        {
            return;
        }

        auto renderer = Renderer::Get<Renderer>();
        renderer->PushScissor(m_Rect);

        auto primaryWindow = Application::Get()->GetWindow();
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
        EASY_BLOCK("PopScissor");

        auto renderer = Renderer::Get<Renderer>();
        auto ret = renderer->PopScissor();
        if (ret.has_value())
        {
            const auto& rect = ret.value();
            auto primaryWindow = Application::Get()->GetWindow();
            glScissor(rect.topLeft.x,
                  primaryWindow->GetHeight() - (rect.topLeft.y + rect.size.y),
                  rect.size.x,
                  rect.size.y);
            GL_CHECK_ERROR()
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
            GL_CHECK_ERROR()
        }



    }
}

#endif
