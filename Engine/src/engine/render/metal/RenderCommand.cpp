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
    void Clear::Execute()
    {
        // TODO
    }

    void SubmitGeo::Execute()
    {
        // TODO
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
        // TODO
    }

    Clear::Clear(bool clearColour, bool clearDepth)
        : m_ClearColour(clearColour)
        , m_ClearDepth(clearDepth)
    {
        // TODO looks like this could be made into common code?
    }

    SubmitGeo::SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer)
        : m_Material(material)
        , m_VertBuffer(vertBuffer)
        , m_IndexBuffer(indexBuffer)
    {
        // TODO looks like this could be made into common code?
    }

    PushScissor::PushScissor(const ui::Rect& rect)
            : m_Rect(rect)
    {
        // TODO looks like this could be made into common code?
    }

    void PushScissor::Execute()
    {
        // TODO
    }

    PopScissor::PopScissor()
    {
        // TODO looks like this could be made into common code?
    }

    void PopScissor::Execute()
    {
        // TODO
    }
}

#endif
