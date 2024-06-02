#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/Mat4.h"
#include "engine/math/Mat3.h"

namespace se::ecs::singleton_components
{
    struct ActiveCameraComponent
    {
        DECLARE_SPARK_SINGLETON_COMPONENT(ActiveCameraComponent)

        se::math::Mat4 proj;
        se::math::Mat4 view;
    };
}
