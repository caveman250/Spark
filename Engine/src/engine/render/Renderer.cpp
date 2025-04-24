#include "engine/render/opengl/GL_fwd.h"
#include "Renderer.h"

#include <engine/Application.h>
#include <engine/ui/singleton_components/UIRenderComponent.h>

#include "engine/profiling/Profiler.h"
#include "metal/MetalRenderer.h"
#include "opengl/OpenGLRenderer.h"
#include "FrameBuffer.h"

DEFINE_SPARK_ENUM_BEGIN(se::render::RenderAPI)
    DEFINE_ENUM_VALUE(RenderAPI, OpenGL)
    DEFINE_ENUM_VALUE(RenderAPI, Metal)
DEFINE_SPARK_ENUM_END()

namespace se::render
{
    Renderer* Renderer::s_Renderer = nullptr;

    Renderer * Renderer::Create()
    {
#if SPARK_PLATFORM_WINDOWS || SPARK_PLATFORM_LINUX
        s_Renderer = new opengl::OpenGLRenderer();
#elif SPARK_PLATFORM_MAC
        s_Renderer = new metal::MetalRenderer();
#endif
        s_Renderer->Init();
        return s_Renderer;
    }

    void Renderer::Shutdown()
    {
        delete s_Renderer;
    }

    void Renderer::SetLastRenderState(const RenderState& rs)
    {
        m_CachedRenderState = rs;
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

    void Renderer::Render()
    {
        PROFILE_SCOPE("Renderer::Render")
        SortDrawCommands();
        ExecuteDrawCommands();
    }

    void Renderer::EndFrame()
    {
        m_RenderCommands.clear();
        m_RenderCommandsArena.Reset();
    }

    void Renderer::SortDrawCommands()
        PROFILE_SCOPE("Renderer::SortDrawCommands")
    {
        std::ranges::stable_sort(m_RenderCommands, [](const auto& lhs, const auto& rhs)
        {
            if (lhs->GetRenderStage() != rhs->GetRenderStage())
            {
                return lhs->GetRenderStage() < rhs->GetRenderStage();
            }

            return lhs->GetSortKey() < rhs->GetSortKey();
        });
    }

    void Renderer::ExecuteDrawCommands()
    {
        PROFILE_SCOPE("Renderer::ExecuteDrawCommands")
        for (const auto& renderCmd : m_RenderCommands)
        {
            renderCmd->Execute();
        }
    }

    void Renderer::Submit(commands::RenderCommand *renderCommand)
    {
        m_RenderCommands.push_back(renderCommand);
    }

    void Renderer::PushFrameBuffer(const std::shared_ptr<FrameBuffer>& fb)
    {
        m_FrameBufferStack.push_back(fb);
        fb->OnPushed();
    }

    void Renderer::PopFrameBuffer()
    {
        auto& fb = m_FrameBufferStack.back();
        fb->OnPopped();
        m_FrameBufferStack.pop_back();
    }
}
