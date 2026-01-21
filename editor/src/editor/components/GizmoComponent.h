#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/math/math.h"
#include "engine/ecs/Signal.h"

namespace se::editor::components 
{
    enum class GizmoAxis
    {
        Forward,
        Right,
        Up
    };

    struct GizmoComponent : ecs::Component
    {
        SPARK_COMPONENT()

        math::Vec3 color = {};
        GizmoAxis axis = GizmoAxis::Forward;
        bool mouseDown = false;
        ecs::Signal<math::Vec3> onMove = {};
    };
}