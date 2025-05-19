#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/Mat4.h"
#include "engine/math/Mat3.h"

namespace se::camera
{
    struct ActiveCameraComponent : reflect::ObjectBase
    {
        SPARK_SINGLETON_COMPONENT();

        SPARK_MEMBER()
        math::Vec3 pos = math::Vec3(0, 2.f, 10.f);

        SPARK_MEMBER()
        math::Vec3 rot = { 0.f, 3.14f, 0.f };

        SPARK_MEMBER()
        se::math::Mat4 proj = {};

        SPARK_MEMBER()
        se::math::Mat4 view = {};

        SPARK_MEMBER()
        int lastMouseX = {};

        SPARK_MEMBER()
        int lastMouseY = {};
    };
}
