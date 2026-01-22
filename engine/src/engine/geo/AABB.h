#pragma once 

#include "spark.h"
#include "engine/math/math.h"

namespace se::geo
{
    struct AABB
    {
        SPARK_POD_CLASS()

        math::Vec3 pos = {};
        math::Vec3 size = {};
    };
}