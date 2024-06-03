#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/Mat4.h"
#include "engine/math/Mat3.h"

namespace se::camera
{
    struct ActiveCameraComponent
    {
        DECLARE_SPARK_SINGLETON_COMPONENT(ActiveCameraComponent)

        math::Vec3 pos = math::Vec3(0, 2.f, 10.f);
        float horizontalAngle = 3.14f;
        float verticalAngle = 0.0f;
        se::math::Mat4 proj = {};
        se::math::Mat4 view = {};
    };
}
