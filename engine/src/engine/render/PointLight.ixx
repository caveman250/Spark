module;

#include "spark.h"

export module Spark.Render.PointLight;

import Spark.Math;

namespace se::render
{
    struct PointLight
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        math::Vec3 pos;

        SPARK_MEMBER(Serialized)
        math::Vec3 color;

        SPARK_MEMBER(Serialized)
        float power;
    };
}
