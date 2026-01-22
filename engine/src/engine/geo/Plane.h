#pragma once

#include "spark.h"
#include "engine/math/Vec3.h"

namespace se::geo
{
    struct Plane
    {
        SPARK_POD_CLASS()

        math::Vec3 normal = {};
        float distSquared = {};
    };
}
