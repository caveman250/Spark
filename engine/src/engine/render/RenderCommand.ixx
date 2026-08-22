module;
#include "spark.h"

export module Spark.Render.RenderCommand;
import Spark.Render.RenderState;
import Spark.Math;
import MaterialInstance;
import Spark.Render.VertexBuffer;
import Spark.Render.IndexBuffer;
import Spark.UI.Rect;

namespace se::render::commands
{
    SPARK_ENUM()
    enum class RenderStage
    {
        Clear,
        Scene,
        UI
    };

    export class RenderCommand
    {
    public:
        virtual ~RenderCommand() {}
        virtual void Execute() = 0;
        virtual RenderStage GetRenderStage() const = 0;
        virtual const RenderState& GetRenderState() const = 0;
        virtual math::Vec3 GetPos() const { return {}; }
    };

    export class Clear : public RenderCommand
    {
    public:
        Clear(bool clearColour, bool clearDepth, const math::Vec4& colour);
        void Execute() override;

        RenderStage GetRenderStage() const override { return RenderStage::Clear; }
        const RenderState& GetRenderState() const override;

        bool m_ClearColour = {};
        bool m_ClearDepth = {};
        math::Vec4 m_Colour = {};
    };

    export class SubmitGeo : public RenderCommand
    {
    public:
        SubmitGeo(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer, const math::Vec3& pos);
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::Scene; }
        const RenderState& GetRenderState() const override;
        math::Vec3 GetPos() const override;

        std::shared_ptr<MaterialInstance> m_MaterialInstance = {};
        std::shared_ptr<VertexBuffer> m_VertBuffer = {};
        std::shared_ptr<IndexBuffer> m_IndexBuffer = {};
        math::Vec3 m_Pos = {};
    };

    export class SubmitUI : public RenderCommand
    {
    public:
        SubmitUI(const std::shared_ptr<MaterialInstance>& materialInstance, const std::shared_ptr<VertexBuffer>& vertBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::UI; }
        const RenderState& GetRenderState() const override;

        std::shared_ptr<MaterialInstance> m_MaterialInstance = {};
        std::shared_ptr<VertexBuffer> m_VertBuffer = {};
        std::shared_ptr<IndexBuffer> m_IndexBuffer = {};
    };

    export class PushScissor : public RenderCommand
    {
    public:
        PushScissor(const ui::Rect& rect);
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::UI; }
        const RenderState& GetRenderState() const override;
    private:
        ui::Rect m_Rect;
    };

    export class PopScissor : public RenderCommand
    {
    public:
        PopScissor();
        void Execute() override;
        RenderStage GetRenderStage() const override { return RenderStage::UI; }
        const RenderState& GetRenderState() const override;
    };
}
