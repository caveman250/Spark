#pragma once

#include "spark.h"
#include "engine/math/math.h"
#include "engine/reflect/Reflect.h"

namespace se::render::components
{
    class PointLightComponent : public reflect::ObjectBase
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        math::Vec3 color;
    };
}
