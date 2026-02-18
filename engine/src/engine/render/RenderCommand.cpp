#include "RenderCommand.h"

#include "Material.h"
#include "MaterialInstance.h"

namespace se::render::commands
{
    Clear::Clear(const bool clearColour,
                 const bool clearDepth)
        : m_ClearColour(clearColour)
        , m_ClearDepth(clearDepth)
    {
    }

    const RenderState& Clear::GetRenderState() const
    {
        static RenderState renderState;
        return renderState;
    }

    SubmitGeo::SubmitGeo(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
        : m_MaterialInstance(materialInstance)
        , m_VertBuffer(vertBuffer)
        , m_IndexBuffer(indexBuffer)
    {
    }

    const RenderState& SubmitGeo::GetRenderState() const
    {
        return m_MaterialInstance->GetMaterial()->GetRenderState();
    }

    SubmitUI::SubmitUI(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer> &vertBuffer,
                       const std::shared_ptr<IndexBuffer> &indexBuffer)
            : m_MaterialInstance(materialInstance)
            , m_VertBuffer(vertBuffer)
            , m_IndexBuffer(indexBuffer)
    {
    }

    const RenderState& SubmitUI::GetRenderState() const
    {
        // Ensure UI commands aren't sorted by render state.
        static RenderState renderState;
        return renderState;
    }

    PushScissor::PushScissor(const ui::Rect& rect)
        : m_Rect(rect)
    {
    }

    const RenderState& PushScissor::GetRenderState() const
    {
        // Ensure UI commands aren't sorted by render state.
        static RenderState renderState;
        return renderState;
    }

    PopScissor::PopScissor()
    {
    }

    const RenderState& PopScissor::GetRenderState() const
    {
        // Ensure UI commands aren't sorted by render state.
        static RenderState renderState;
        return renderState;
    }
}
