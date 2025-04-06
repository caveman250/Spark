#pragma once
#include "engine/reflect/Reflect.h"

namespace se::ui
{
    struct Anchors
    {
        DECLARE_SPARK_POD_CLASS(Anchors)

        float left = 0.f;
        float right = 0.f;
        float top = 0.f;
        float bottom = 0.f;
    };
}
