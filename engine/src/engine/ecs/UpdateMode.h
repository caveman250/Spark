#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    SPARK_ENUM()
    enum class UpdateMode : int
    {
        SingleThreaded,
        MultiThreaded
    };
}