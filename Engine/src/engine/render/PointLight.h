#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::render
{
    struct PointLight
    {
        SPARK_POD_CLASS(PointLight)

        SPARK_MEMBER(Serialized)
        math::Vec3 pos;

        SPARK_MEMBER(Serialized)
        math::Vec3 color;
    };
}
