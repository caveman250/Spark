#pragma once

#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::render
{
    struct DirLight
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        math::Vec3 dir;

        SPARK_MEMBER(Serialized)
        math::Vec3 color;
    };
}
