#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    DECLARE_SPARK_ENUM_BEGIN(MouseButton, int)
        Unknown,
        Left,
        Right,
        Middle,
        Btn4,
        Btn5,
        None
    DECLARE_SPARK_ENUM_END()
}
