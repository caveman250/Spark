module;

#include "spark.h"

export module Spark.Geo.AABB;
import Spark.Math;

namespace se::geo
{
    export struct AABB
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        math::Vec3 center = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 size = {};
    };
}