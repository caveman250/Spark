#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    DECLARE_SPARK_ENUM_BEGIN(KeyState, int)
        Up = 1,
        Down = 2
    DECLARE_SPARK_ENUM_END()
}
