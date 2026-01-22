#pragma once

#include "engine/ecs/Component.h"
#include "engine/math/Mat4.h"
#include "engine/math/Mat3.h"

namespace se::camera
{
    struct ActiveCameraComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT();

        SPARK_MEMBER()
        math::Vec3 pos = {};

        SPARK_MEMBER()
        math::Vec3 rot = {};

        SPARK_MEMBER()
        math::Mat4 proj = {};

        SPARK_MEMBER()
        math::Mat4 view = {};

        SPARK_MEMBER()
        int lastMouseX = {};

        SPARK_MEMBER()
        int lastMouseY = {};
    };
}
