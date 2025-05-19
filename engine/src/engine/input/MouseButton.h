#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    SPARK_ENUM_BEGIN(MouseButton, int)
        Unknown,
        Left,
        Right,
        Middle,
        Btn4,
        Btn5,
        None
    SPARK_ENUM_END()
}
