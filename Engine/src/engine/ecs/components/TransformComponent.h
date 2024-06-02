#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs::components
{
    struct TransformComponent
    {
        DECLARE_SPARK_COMPONENT(TransformComponent)

        math::Vec3 pos;
        math::Vec3 rot;
        math::Vec3 scale;
    };
}
