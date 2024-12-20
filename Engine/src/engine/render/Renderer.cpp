#include "engine/render/opengl/GL_fwd.h"
#include "Renderer.h"

#include <engine/Application.h>
#include <engine/ui/singleton_components/UIRenderComponent.h>

#include "engine/profiling/Profiler.h"
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
        PROFILE_SCOPE("Renderer::Update")
        m_LightSetup.Reset();
    }

    void Renderer::Render(IWindow* window)
    {
        PROFILE_SCOPE("Renderer::Render")
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
        PROFILE_SCOPE("Renderer::SortDrawCommands")
        auto& renderCmds = m_RenderCommands[window];
        std::ranges::stable_sort(renderCmds, [](const auto& lhs, const auto& rhs)
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
        PROFILE_SCOPE("Renderer::ExecuteDrawCommands")
        for (const auto& renderCmd : m_RenderCommands[window])
        {
            renderCmd->Execute();
        }
    }

    void Renderer::Submit(IWindow *window, commands::RenderCommand *renderCommand)
    {
        m_RenderCommands[window].push_back(renderCommand);
    }
}
