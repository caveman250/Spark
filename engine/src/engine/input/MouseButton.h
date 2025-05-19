#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    SPARK_ENUM()
    enum class MouseButton : int
    {
        Unknown,
        Left,
        Right,
        Middle,
        Btn4,
        Btn5,
        None
    };
}
