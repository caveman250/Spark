#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    SPARK_ENUM_BEGIN(UpdateMode, int)
        SingleThreaded,
        MultiThreaded
    SPARK_ENUM_END()
}