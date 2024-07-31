#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    DECLARE_SPARK_ENUM_BEGIN(KeyState, uint8_t)
        Up,
        Down
    DECLARE_SPARK_ENUM_END()
}
