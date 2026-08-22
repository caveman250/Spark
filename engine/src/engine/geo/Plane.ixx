module;

#include "spark.h"

export module Spark.Geo.Plane;
import Spark.Math;

namespace se::geo
{
    export struct Plane
    {
        SPARK_POD_CLASS()

        math::Vec3 normal = {};
        math::Vec3 center = {};
    };
}
