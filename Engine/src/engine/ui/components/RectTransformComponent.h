#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Anchors.h"
#include "engine/ui/Rect.h"
#include "engine/math/math.h"

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

        float minAspectRatio = 0.f;
        float maxAspectRatio = 0.f;

        Rect rect = {};
        Rect lastRect = {};
        math::Vec2 desiredSize = {};
        bool overridesChildSizes = false;

        int layer = 0;

        bool needsLayout = false; // runtime only
    };
}
