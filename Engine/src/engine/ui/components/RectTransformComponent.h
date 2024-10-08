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
        int minX = 0;
        int minY = 0;
        int maxX = 0;
        int maxY = 0;

        Rect rect = {};
        Rect lastRect = {};

        int layer = 0;

        bool needsLayout = true; // runtime only
    };
}
