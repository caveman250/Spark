#pragma once
#include "engine/math/Vec2.h"
#include "engine/reflect/Reflect.h"

namespace se::ui
{
    class Rect 
    {
        DECLARE_SPARK_POD_CLASS(Rect)

        math::Vec2 topLeft;
        math::Vec2 bottomRight;
    };
}
