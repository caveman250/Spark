#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::ecs::components
{
    struct TransformComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(TransformComponent)

        math::Vec3 pos = {};
        math::Vec3 rot = {};
        math::Vec3 scale = math::Vec3(1.f, 1.f, 1.f);

        math::Mat4 transform = {};
        math::Mat4 worldTransform = {};
    };
}
