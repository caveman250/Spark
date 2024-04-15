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

namespace se::render
{
    struct RenderCommand
    {
        RenderCommand(const std::function<void()>& cmd) : command(cmd) {}
        std::function<void()> command;

        static void Clear();
        static void SubmitGeo(const std::shared_ptr<Material>& material, const std::shared_ptr<VertexBuffer>& vertBuffer, int indexCount);
    };
}
