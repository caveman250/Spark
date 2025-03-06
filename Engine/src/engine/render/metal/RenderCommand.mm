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

// TODO move this somewhere common
DEFINE_SPARK_ENUM_BEGIN(se::render::commands::RenderStage)
                        DEFINE_ENUM_VALUE(RenderStage, Scene)
                        DEFINE_ENUM_VALUE(RenderStage, UI)
DEFINE_SPARK_ENUM_END()

namespace se::render::commands
{
    Clear::Clear(bool clearColour, bool clearDepth)
            : m_ClearColour(clearColour)
            , m_ClearDepth(clearDepth)
    {
        // TODO looks like this could be made into common code?
    }

    void Clear::Execute()
    {
        // nothing to do here
    }

    SubmitGeo::SubmitGeo(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
            : m_MaterialInstance(materialInstance)
            , m_VertBuffer(vertBuffer)
            , m_IndexBuffer(indexBuffer)
    {
        // TODO looks like this could be made into common code?
    }

    void SubmitGeo::Execute()
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

    SubmitUI::SubmitUI(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer> &vertBuffer,
                       const std::shared_ptr<IndexBuffer> &indexBuffer)
            : m_MaterialInstance(materialInstance)
            , m_VertBuffer(vertBuffer)
            , m_IndexBuffer(indexBuffer)
    {
        // TODO looks like this could be made into common code?
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

    PushScissor::PushScissor(const ui::Rect& rect)
            : m_Rect(rect)
    {
        // TODO looks like this could be made into common code?
    }

    void PushScissor::Execute()
    {
        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        auto window = Application::Get()->GetPrimaryWindow();
        MTLScissorRect scissor;
        scissor.x = std::max(0, m_Rect.topLeft.x);
        scissor.y = std::max(0, m_Rect.topLeft.y);
        scissor.width = std::min(m_Rect.size.x, window->GetWidth() - static_cast<int>(scissor.x));
        scissor.height = std::min(m_Rect.size.y, window->GetHeight() - static_cast<int>(scissor.y));
        [commandEncoder setScissorRect:scissor];
    }

    PopScissor::PopScissor()
    {
        // TODO looks like this could be made into common code?
    }

    void PopScissor::Execute()
    {
        auto primaryWindow = Application::Get()->GetPrimaryWindow(); // TODO
        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        MTLScissorRect scissor;
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = primaryWindow->GetWidth();
        scissor.height = primaryWindow->GetHeight();
        [commandEncoder setScissorRect:scissor];
    }
}

#endif
