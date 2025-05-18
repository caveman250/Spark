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

        SPARK_MEMBER(Serialized)
        Anchors anchors = {};

        SPARK_MEMBER(Serialized)
        int minX = 0;

        SPARK_MEMBER(Serialized)
        int minY = 0;

        SPARK_MEMBER(Serialized)
        int maxX = 0;

        SPARK_MEMBER(Serialized)
        int maxY = 0;

        SPARK_MEMBER(Serialized)
        float minAspectRatio = 0.f;

        SPARK_MEMBER(Serialized)
        float maxAspectRatio = 0.f;

        SPARK_MEMBER()
        Rect rect = {};

        SPARK_MEMBER()
        Rect lastRect = {};

        SPARK_MEMBER()
        math::Vec2 desiredSize = {};

        SPARK_MEMBER()
        bool overridesChildSizes = false;

        SPARK_MEMBER()
        int layer = 0;

        SPARK_MEMBER()
        bool needsLayout = false;
    };
}
