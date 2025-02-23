#include "IndexBuffer.h"
#include "spark.h"
#include "engine/render/render_fwd.h"
#if METAL_RENDERER

#include "engine/render/RenderCommand.h"
#include "engine/Application.h"
#include "engine/render/Material.h"
#include "engine/render/VertexBuffer.h"

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

    SubmitGeo::SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
    : m_Material(material)
    , m_VertBuffer(vertBuffer)
    , m_IndexBuffer(indexBuffer)
    {
        // TODO looks like this could be made into common code?
    }

    void SubmitGeo::Execute()
    {
        m_Material->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();

        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        auto* mtlBuffer = static_cast<metal::IndexBuffer*>(m_IndexBuffer.get());
        commandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
            m_IndexBuffer->GetIndices().size(),
            MTL::IndexTypeUInt32,
            mtlBuffer->GetMTLBuffer(),
            0,
            1);
    }

    SubmitUI::SubmitUI(const std::shared_ptr<Material> &material, const std::shared_ptr<VertexBuffer> &vertBuffer,
        const std::shared_ptr<IndexBuffer> &indexBuffer)
        : m_Material(material)
        , m_VertBuffer(vertBuffer)
        , m_IndexBuffer(indexBuffer)
    {
        // TODO looks like this could be made into common code?
    }

    void SubmitUI::Execute()
    {
        m_Material->Bind(*m_VertBuffer);
        m_VertBuffer->Bind();

        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        auto* mtlBuffer = static_cast<metal::IndexBuffer*>(m_IndexBuffer.get());
        commandEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
            m_IndexBuffer->GetIndices().size(),
            MTL::IndexTypeUInt32,
            mtlBuffer->GetMTLBuffer(),
            0,
            1);
    }

    PushScissor::PushScissor(const ui::Rect& rect)
            : m_Rect(rect)
    {
        // TODO looks like this could be made into common code?
    }

    void PushScissor::Execute()
    {
        auto primaryWindow = Application::Get()->GetPrimaryWindow(); // TODO
        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        MTL::ScissorRect scissor;
        scissor.x = m_Rect.topLeft.x;
        scissor.y = m_Rect.topLeft.y;
        scissor.width = m_Rect.size.x;
        scissor.height = m_Rect.size.y;
        commandEncoder->setScissorRect(scissor);
    }

    PopScissor::PopScissor()
    {
        // TODO looks like this could be made into common code?
    }

    void PopScissor::Execute()
    {
        auto primaryWindow = Application::Get()->GetPrimaryWindow(); // TODO
        auto commandEncoder = Renderer::Get<metal::MetalRenderer>()->GetCurrentCommandEncoder();
        MTL::ScissorRect scissor;
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = primaryWindow->GetWidth();
        scissor.height = primaryWindow->GetHeight();
        commandEncoder->setScissorRect(scissor);
    }
}

#endif
