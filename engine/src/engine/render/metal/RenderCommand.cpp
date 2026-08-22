module;

#include "engine/render/render_fwd.h"
#if METAL_RENDERER
#include "wrappers/RenderCommand.h"

#include "spark.h"
#include "platform/mac/Window.h"
#include "engine/render/DrawMode.h"
#include "platform/mac/wrappers/RunLoop.h"

module Spark.Render.RenderCommand;
import Spark.Render.IndexBuffer;
import Spark.Render.Renderer;
import Spark.Render.Metal.MetalRenderer;
import Spark.Render.Metal.IndexBuffer;
import Spark.Render.Metal.VertexBuffer;
import Spark.Render.VertexBuffer;
import Application;


namespace se::render::commands
{
    void Clear::Execute()
    {
        Renderer::Get<metal::MetalRenderer>()->SetClearColour(m_Colour);
    }

    MTLPrimitiveType DrawModeToMTLPrimitiveType(DrawMode mode)
    {
        switch (mode)
        {
            case DrawMode::Triangles:
                return MTLPrimitiveTypeTriangle;
            case DrawMode::Lines:
                return MTLPrimitiveTypeLine;
            default:
                SPARK_ASSERT(false);
                return MTLPrimitiveTypeTriangle;
        }
    }

    void SubmitGeo::Execute()
    {
        m_MaterialInstance->Bind(m_VertBuffer);
        m_VertBuffer->Bind();
        MTLRenderCommandEncoderPtr commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        if (m_IndexBuffer)
        {
            metal::wrappers::RenderCommand::RenderIndexedGeo(commandEncoder,
                DrawModeToMTLPrimitiveType(m_MaterialInstance->GetMaterial()->GetRenderState().drawMode),
                static_cast<metal::IndexBuffer*>(m_IndexBuffer.get())->GetMTLBuffer(),
                m_IndexBuffer->GetIndices().size());
        }
        else
        {
            metal::wrappers::RenderCommand::RenderIndexedGeo(commandEncoder,
                DrawModeToMTLPrimitiveType(m_MaterialInstance->GetMaterial()->GetRenderState().drawMode),
                static_cast<metal::VertexBuffer*>(m_VertBuffer.get())->GetMTLBuffer(0),
                m_VertBuffer->GetVertexCount());
        }

    }

    void SubmitUI::Execute()
    {
        m_MaterialInstance->Bind(m_VertBuffer);
        m_VertBuffer->Bind();

        MTLRenderCommandEncoderPtr commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();

        metal::wrappers::RenderCommand::RenderIndexedGeo(commandEncoder,
             DrawModeToMTLPrimitiveType(m_MaterialInstance->GetMaterial()->GetRenderState().drawMode),
             static_cast<metal::IndexBuffer*>(m_IndexBuffer.get())->GetMTLBuffer(),
             m_IndexBuffer->GetIndices().size());
    }

    void PushScissor::Execute()
    {
        auto renderer = Renderer::Get<Renderer>();

        auto currScissor = renderer->PeekScissor();
        renderer->PushScissor(m_Rect);
        if (currScissor.has_value() && !currScissor.value().Contains(m_Rect))
        {
            return;
        }

        if (m_Rect.size.x < 0 || m_Rect.size.y < 0)
        {
            return;
        }
        
        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        auto* window = Application::Get()->GetWindow();
        MTLScissorRect scissor;
        scissor.x = std::max(0, m_Rect.topLeft.x);
        scissor.y = std::max(0, m_Rect.topLeft.y);
        scissor.width = std::min(m_Rect.size.x, window->GetWidth() - static_cast<int>(scissor.x));
        scissor.height = std::min(m_Rect.size.y, window->GetHeight() - static_cast<int>(scissor.y));
        metal::wrappers::RenderCommand::SetScissorRect(commandEncoder, scissor);
    }

    void PopScissor::Execute()
    {
        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();

        auto renderer = Renderer::Get<Renderer>();
        auto ret = renderer->PopScissor();

        if (ret.has_value())
        {
            const auto& rect = ret.value();
            if (rect.size.x <= 0 || rect.size.y <= 0)
            {
                return;
            }

            auto* window = Application::Get()->GetWindow();
            MTLScissorRect scissor;
            scissor.x = std::max(0, rect.topLeft.x);
            scissor.y = std::max(0, rect.topLeft.y);
            scissor.width = std::min(rect.size.x, window->GetWidth() - static_cast<int>(scissor.x));
            scissor.height = std::min(rect.size.y, window->GetHeight() - static_cast<int>(scissor.y));
            metal::wrappers::RenderCommand::SetScissorRect(commandEncoder, scissor);
        }
        else
        {
            auto* window = Application::Get()->GetWindow();
            MTLScissorRect scissor;
            scissor.x = 0;
            scissor.y = 0;
            scissor.width = window->GetWidth();
            scissor.height = window->GetHeight();
            metal::wrappers::RenderCommand::SetScissorRect(commandEncoder, scissor);
        }
    }
}

#endif
