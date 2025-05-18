#include "RenderCommand.h"

namespace se::render::commands
{
    Clear::Clear(bool clearColour, bool clearDepth)
            : m_ClearColour(clearColour)
            , m_ClearDepth(clearDepth)
    {
    }

    SubmitGeo::SubmitGeo(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
            : m_MaterialInstance(materialInstance)
            , m_VertBuffer(vertBuffer)
            , m_IndexBuffer(indexBuffer)
    {
    }

    SubmitUI::SubmitUI(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer> &vertBuffer,
                       const std::shared_ptr<IndexBuffer> &indexBuffer)
            : m_MaterialInstance(materialInstance)
            , m_VertBuffer(vertBuffer)
            , m_IndexBuffer(indexBuffer)
    {
    }

    PushScissor::PushScissor(const ui::Rect& rect)
            : m_Rect(rect)
    {
    }

    PopScissor::PopScissor()
    {
    }
}
