#import "engine/render/RenderCommand.h"

#include "IndexBuffer.h"
#include "spark.h"
#include "engine/render/render_fwd.h"
#if METAL_RENDERER

#include "engine/render/RenderCommand.h"
#include "engine/Application.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/VertexBuffer.h"
#import "engine/render/metal/MetalRenderer.h"
#import <Metal/Metal.h>
#import "platform/mac/Window.h"
#include "VertexBuffer.h"

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
        m_MaterialInstance->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();

        id<MTLRenderCommandEncoder> commandEncoder = static_cast<id<MTLRenderCommandEncoder>>(Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder());
        if (m_IndexBuffer)
        {
            auto* mtlBuffer = static_cast<metal::IndexBuffer*>(m_IndexBuffer.get());
            [commandEncoder drawIndexedPrimitives:DrawModeToMTLPrimitiveType(m_MaterialInstance->GetMaterial()->GetRenderState().drawMode)
                                       indexCount:m_IndexBuffer->GetIndices().size()
                                       indexType:MTLIndexTypeUInt32
                                       indexBuffer:(id<MTLBuffer>)mtlBuffer->GetMTLBuffer()
                                       indexBufferOffset:0
                                       instanceCount:1];
        }
        else
        {
            auto* mtlBuffer = static_cast<metal::VertexBuffer*>(m_VertBuffer.get());
            [commandEncoder setVertexBuffer:(id<MTLBuffer>)mtlBuffer->GetMTLBuffer(0)
                  offset:0
                 atIndex:0];

            [commandEncoder drawPrimitives:DrawModeToMTLPrimitiveType(m_MaterialInstance->GetMaterial()->GetRenderState().drawMode)
                        vertexStart:0
                        vertexCount:mtlBuffer->GetVertexCount()];
        }

    }

    void SubmitUI::Execute()
    {
        m_MaterialInstance->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();

        id<MTLRenderCommandEncoder> commandEncoder = static_cast<id<MTLRenderCommandEncoder>>(Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder());
        auto* mtlBuffer = static_cast<metal::IndexBuffer*>(m_IndexBuffer.get());
        [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                   indexCount:m_IndexBuffer->GetIndices().size()
                                    indexType:MTLIndexTypeUInt32
                                  indexBuffer:(id<MTLBuffer>)mtlBuffer->GetMTLBuffer()
                            indexBufferOffset:0
                                instanceCount:1];
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
        auto window = Application::Get()->GetWindow();
        MTLScissorRect scissor;
        scissor.x = std::max(0, m_Rect.topLeft.x);
        scissor.y = std::max(0, m_Rect.topLeft.y);
        scissor.width = std::min(m_Rect.size.x, window->GetWidth() - static_cast<int>(scissor.x));
        scissor.height = std::min(m_Rect.size.y, window->GetHeight() - static_cast<int>(scissor.y));
        [commandEncoder setScissorRect:scissor];
    }

    void PopScissor::Execute()
    {
        auto primaryWindow = Application::Get()->GetWindow();
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

            MTLScissorRect scissor;
            scissor.x = std::max(0, rect.topLeft.x);
            scissor.y = std::max(0, rect.topLeft.y);
            scissor.width = std::min(rect.size.x, primaryWindow->GetWidth() - static_cast<int>(scissor.x));
            scissor.height = std::min(rect.size.y, primaryWindow->GetHeight() - static_cast<int>(scissor.y));
            [commandEncoder setScissorRect:scissor];
        }
        else
        {
            MTLScissorRect scissor;
            scissor.x = 0;
            scissor.y = 0;
            scissor.width = primaryWindow->GetWidth();
            scissor.height = primaryWindow->GetHeight();
            [commandEncoder setScissorRect:scissor];
        }
    }
}

#endif
