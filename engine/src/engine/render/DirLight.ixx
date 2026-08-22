module;

#include "spark.h"

export module Spark.Render.DirLight;
import Spark.Math;

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
