#pragma once
#include "spark.h"

namespace se::render
{
    class Material;
    class VertexBuffer;
}

namespace se
{
    class IWindow;
}

namespace se::render::commands
{
    class RenderCommand
    {
    public:
        virtual ~RenderCommand() {}
        virtual void Execute() = 0;
    };

    class Clear : public RenderCommand
    {
    public:
        Clear(bool clearColour, bool clearDepth);
        void Execute() override;

    private:
        bool m_ClearColour = {};
        bool m_ClearDepth = {};
    };

    class SubmitGeo : public RenderCommand
    {
    public:
        SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, int indexCount);
        void Execute() override;

    private:
        std::shared_ptr<Material> m_Material = {};
        std::shared_ptr<VertexBuffer> m_VertBuffer = {};
        int m_IndexCount = {};
    };
}
