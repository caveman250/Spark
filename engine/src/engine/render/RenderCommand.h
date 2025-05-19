#pragma once
#include "spark.h"
#include "IndexBuffer.h"
#include "engine/ui/Rect.h"

namespace se::render
{
    class MaterialInstance;
    class VertexBuffer;
}

namespace se
{
    class IWindow;
}

namespace se::render::commands
{
    SPARK_ENUM()
    enum class RenderStage
    {
        Clear,
        Scene,
        UI
    };

    class RenderCommand
    {
    public:
        virtual ~RenderCommand() {}
        virtual void Execute() = 0;
        virtual RenderStage GetRenderStage() const = 0;
        virtual uint32_t GetSortKey() const { return 0; }
    };

    class Clear : public RenderCommand
    {
    public:
        Clear(bool clearColour, bool clearDepth);
        void Execute() override;

        RenderStage GetRenderStage() const override { return RenderStage::Clear; }

    private:
        bool m_ClearColour = {};
        bool m_ClearDepth = {};
    };

    class SubmitGeo : public RenderCommand
    {
    public:
        SubmitGeo(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::Scene; }

    private:
        std::shared_ptr<MaterialInstance> m_MaterialInstance = {};
        std::shared_ptr<VertexBuffer> m_VertBuffer = {};
        std::shared_ptr<IndexBuffer> m_IndexBuffer = {};
    };

    class SubmitUI : public RenderCommand
    {
    public:
        SubmitUI(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::UI; }

    private:
        std::shared_ptr<MaterialInstance> m_MaterialInstance = {};
        std::shared_ptr<VertexBuffer> m_VertBuffer = {};
        std::shared_ptr<IndexBuffer> m_IndexBuffer = {};

    };

    class PushScissor : public RenderCommand
    {
    public:
        PushScissor(const ui::Rect& rect);
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::UI; }
    private:
        ui::Rect m_Rect;
    };

    class PopScissor : public RenderCommand
    {
    public:
        PopScissor();
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::UI; }
    };
}
