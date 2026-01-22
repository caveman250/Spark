#pragma once

#include "spark.h"
#include "engine/math/math.h"

namespace se::geo
{
    struct Ray
    {
        SPARK_POD_CLASS()

        math::Vec3 origin = {};
        math::Vec3 direction = {};
    };
}
