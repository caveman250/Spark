module;
#include "spark.h"


export module Spark.Render.RenderGroup;
import Spark.Render.FrameBuffer;
import Spark.Render.RenderCommand;

namespace se::render
{
    class Material;
    struct RenderGroup
    {
        std::vector<commands::RenderCommand*> renderCommands = {};
        std::shared_ptr<FrameBuffer> frameBuffer = nullptr;
        Material* boundMaterial = {};
        int layer = 0;
    };
}
