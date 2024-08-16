#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Anchors.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct RectTransformComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(RectTransformComponent)

        Anchors anchors = {};
        float minX = 0;
        float minY = 0;
        float maxX = 0;
        float maxY = 0;

        Rect rect = {};

        bool needsArrange = true; // runtime only
    };
}
