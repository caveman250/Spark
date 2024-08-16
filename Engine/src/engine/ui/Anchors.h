#pragma once
#include "engine/reflect/Reflect.h"

namespace se::ui
{
    struct Anchors
    {
        DECLARE_SPARK_POD_CLASS(Anchors)

        float left;
        float right;
        float top;
        float bottom;
    };
}
