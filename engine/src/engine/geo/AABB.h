#pragma once 

#include "spark.h"
#include "engine/math/math.h"

namespace se::geo
{
    struct AABB
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        math::Vec3 pos = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 size = {};
    };
}