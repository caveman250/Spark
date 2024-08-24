#include <engine/render/opengl/GL_fwd.h>
#include "Renderer.h"
#include "opengl/OpenGLRenderer.h"

namespace se::render
{
    static Renderer* s_Renderer = nullptr;

    Renderer * Renderer::Create()
    {
        s_Renderer = new opengl::OpenGLRenderer();
        s_Renderer->Init();
        return s_Renderer;
    }

    void Renderer::Shutdown()
    {
        delete s_Renderer;
    }

    Renderer * Renderer::Get()
    {
        return s_Renderer;
    }

    void Renderer::ApplyRenderState(const RenderState& rs)
    {
        if (rs != m_CachedRenderState)
        {
            ApplyDepthCompare(rs.depthComp);
            ApplyBlendMode(rs.srcBlend, rs.dstBlend);
            m_CachedRenderState = rs;
        }
    }

    void Renderer::AddPointLight(const PointLight& light)
    {
        m_LightSetup.pointLights.push_back(light);
    }

    void Renderer::Update()
    {
        m_LightSetup.Reset();
    }

    void Renderer::Render(IWindow* window)
    {
        SortDrawCommands(window);
        ExecuteDrawCommands(window);
    }

    void Renderer::EndFrame()
    {
        m_RenderCommands.clear();
        m_RenderCommandsArena.Reset();
    }

    void Renderer::SortDrawCommands(IWindow *window)
    {
        auto& renderCmds = m_RenderCommands[window];
        std::ranges::sort(renderCmds, [](const auto& lhs, const auto& rhs)
        {
            if (lhs->GetRenderStage() != rhs->GetRenderStage())
            {
                return lhs->GetRenderStage() < rhs->GetRenderStage();
            }

            return lhs->GetSortKey() < rhs->GetSortKey();
        });
    }

    void Renderer::ExecuteDrawCommands(IWindow* window)
    {
        for (const auto& renderCmd : m_RenderCommands[window])
        {
            renderCmd->Execute();
        }
    }
}
