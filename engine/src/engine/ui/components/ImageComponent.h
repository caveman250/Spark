#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct ImageComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        Rect lastRect = {};
    };
}