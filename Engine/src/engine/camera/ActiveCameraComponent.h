#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/Mat4.h"
#include "engine/math/Mat3.h"

namespace se::camera
{
    struct ActiveCameraComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_SINGLETON_COMPONENT(ActiveCameraComponent);

        math::Vec3 pos = math::Vec3(0, 2.f, 10.f);
        math::Vec3 rot = { 0.f, 3.14f, 0.f };
        se::math::Mat4 proj = {};
        se::math::Mat4 view = {};
        int lastMouseX = {};
        int lastMouseY = {};
    };
}
