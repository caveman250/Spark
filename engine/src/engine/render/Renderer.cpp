#include "Renderer.h"

#include "FrameBuffer.h"
#include "engine/Application.h"
#include <easy/profiler.h>
#include "opengl/OpenGLRenderer.h"
#include "metal/MetalRenderer.h"

namespace se::render
{
    Renderer *Renderer::s_Renderer = nullptr;

    Renderer *Renderer::Create()
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

    void Renderer::SetLastRenderState(const RenderState &rs)
    {
        m_CachedRenderState = rs;
    }

    void Renderer::AddPointLight(const PointLight &light)
    {
        m_LightSetup.pointLights
                .push_back(light);
    }

    void Renderer::Update()
    {
        EASY_BLOCK("Renderer::Update");
        m_LightSetup.Reset();
        m_DefaultRenderGroup = AllocRenderGroup(0);
        Submit<commands::Clear>(true, true);
    }

    void Renderer::Render()
    {
        EASY_BLOCK("Renderer::Render");
        SortDrawCommands();
        ExecuteDrawCommands();
    }

    void Renderer::EndFrame()
    {
        m_RenderGroups.clear();
        m_RenderCommandsArena.Reset();
    }

    void Renderer::SortDrawCommands()
    {
        EASY_FUNCTION();
        for (auto &group: m_RenderGroups)
        {
            std::ranges::stable_sort(group.renderCommands,[](const auto &lhs, const auto &rhs)
            {
                if (lhs->GetRenderStage() != rhs->GetRenderStage())
                {
                    return lhs->GetRenderStage() < rhs->GetRenderStage();
                }

                return lhs->GetSortKey() < rhs->GetSortKey();
            });
        }
    }

    void Renderer::ExecuteDrawCommands()
    {
        EASY_FUNCTION();
        std::set<render::FrameBuffer*> frameBuffers = {};
        std::ranges::sort(m_RenderGroups, [](const RenderGroup& lhs, const RenderGroup& rhs)
        {
            if (lhs.frameBuffer != rhs.frameBuffer)
            {
                return lhs.frameBuffer < rhs.frameBuffer;
            }

            return lhs.layer < rhs.layer;
        });

        for (const auto& renderGroup : m_RenderGroups)
        {
            if (renderGroup.frameBuffer)
            {
                frameBuffers.insert(renderGroup.frameBuffer.get());
            }
        }

        for (const auto& frameBuffer : frameBuffers)
        {
            frameBuffer->PreRender();
        }

        for (int i = static_cast<int>(m_RenderGroups.size()) - 1; i >= 0; --i)
        {
            auto& renderGroup = m_RenderGroups[i];
            m_ActiveRenderGroup = i;
            if (renderGroup.frameBuffer)
            {
                renderGroup.frameBuffer->Bind();
            }

            for (const auto& renderCmd : renderGroup.renderCommands)
            {
                renderCmd->Execute();
            }

            if (renderGroup.frameBuffer)
            {
                renderGroup.frameBuffer->UnBind();
            }
        }

        for (const auto& frameBuffer : frameBuffers)
        {
            frameBuffer->Commit();
        }
    }

    void Renderer::Submit(size_t group, commands::RenderCommand *renderCommand)
    {
        m_RenderGroups[group].renderCommands.push_back(renderCommand);
    }

    size_t Renderer::AllocRenderGroup(int layer)
    {
        auto& renderGroup = m_RenderGroups.emplace_back();
        renderGroup.layer = layer;
        return m_RenderGroups.size() - 1;
    }

    void Renderer::SetFrameBuffer(size_t group, const std::shared_ptr<FrameBuffer>& fb)
    {
        m_RenderGroups[group].frameBuffer = fb;
    }
}
