module;

#include "spark.h"
#include "engine/ecs/Component.h"


export module TransformGizmoComponent;
import Signal;
import Spark.Math;

namespace se::editor::components 
{
    SPARK_ENUM()
    export enum class GizmoAxis
    {
        Forward,
        Right,
        Up
    };

    export struct TransformGizmoComponent : ecs::Component
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
