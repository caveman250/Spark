#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    SPARK_ENUM()
    enum class KeyState
    {
        Up = 1,
        Down = 2
    };
}
