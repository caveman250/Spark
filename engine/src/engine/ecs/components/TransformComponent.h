#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::ecs::components
{
    struct TransformComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        math::Vec3 pos = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 rot = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 scale = math::Vec3(1.f, 1.f, 1.f);

        SPARK_MEMBER()
        math::Mat4 transform = {};

        SPARK_MEMBER()
        math::Mat4 worldTransform = {};
    };
}
