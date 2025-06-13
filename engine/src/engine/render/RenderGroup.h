#pragma once

#include "spark.h"
#include "RenderCommand.h"
#include "FrameBuffer.h"

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