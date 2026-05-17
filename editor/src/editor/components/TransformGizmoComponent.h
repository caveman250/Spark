#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/math/math.h"
#include "engine/ecs/Signal.h"

namespace se::editor::components 
{
    SPARK_ENUM()
    enum class GizmoAxis
    {
        Forward,
        Right,
        Up
    };

    struct TransformGizmoComponent : ecs::Component
    {
        SPARK_COMPONENT(EditorOnly)

        math::Vec4 color = {};
        GizmoAxis axis = GizmoAxis::Forward;
        bool mouseDown = false;
        bool wasMouseDown = false;
        ecs::Signal<math::Vec3> onMove = {};
        ecs::Signal<> onFinishMove = {};
    };
}