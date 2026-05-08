#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/math/math.h"
#include "engine/ecs/Signal.h"

namespace se::editor::components 
{
    enum class RotationAxis
    {
        X,
        Y,
        Z
    };

    struct RotationGizmoComponent : ecs::Component
    {
        SPARK_COMPONENT(EditorOnly)

        math::Vec4 color = {};
        RotationAxis axis = RotationAxis::X;
        bool mouseDown = false;
        ecs::Signal<math::Vec3> onMove = {};
    };
}