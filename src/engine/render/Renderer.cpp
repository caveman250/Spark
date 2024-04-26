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

    void Renderer::Submit(IWindow* window, const RenderCommand &renderCmd)
    {
        RenderCommands[window].push_back(renderCmd);
    }

    void Renderer::ApplyRenderState(const RenderState& rs)
    {
        if (rs != m_CachedRenderState)
        {
            ApplyDepthCompare(rs.depthComp);
            m_CachedRenderState = rs;
        }
    }

    void Renderer::EndFrame()
    {
        RenderCommands.clear();
    }

    void Renderer::ExecuteDrawCommands(IWindow* window)
    {
        for (const auto& renderCmd : RenderCommands[window])
        {
            renderCmd.command();
        }
    }
}
