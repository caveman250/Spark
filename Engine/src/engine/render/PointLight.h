#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::render
{
    struct PointLight
    {
        DECLARE_SPARK_POD_CLASS(PointLight)

        math::Vec3 pos;
        math::Vec3 color;
    };
}
