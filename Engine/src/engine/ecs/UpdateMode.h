#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ecs
{
    DECLARE_SPARK_ENUM_BEGIN(UpdateMode, int)
        SingleThreaded,
        MultiThreaded
    DECLARE_SPARK_ENUM_END()
}