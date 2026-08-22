module;

#include "spark.h"

export module Spark.Geo.Ray;
import Spark.Math;

namespace se::geo
{
    export struct Ray
    {
        SPARK_POD_CLASS()

        math::Vec3 origin = {};
        math::Vec3 direction = {};
    };
}
