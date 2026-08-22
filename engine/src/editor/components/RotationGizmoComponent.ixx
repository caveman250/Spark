module;

#include "spark.h"
#include "engine/ecs/Component.h"





export module RotationGizmoComponent;
import Signal;
import Spark.Math;
import Spark.Render.VertexBuffer;
import Spark.Render.IndexBuffer;
import Spark.Geo.AABB;

namespace se::editor::components 
{
    SPARK_ENUM()
    export enum class RotationAxis
    {
        X,
        Y,
        Z
    };

    export struct RotationGizmoComponent : ecs::Component
    {
        SPARK_COMPONENT(EditorOnly)

        math::Vec4 color = {};
        math::Vec4 selectedColor = {};
        RotationAxis axis = RotationAxis::X;
        math::Vec3 initialClickPos = {};
        bool wasHovered = true;
        bool wasMouseDown = false;
        bool mouseDown = false;
        bool hasSetVisualisationDir = false;
        float visualisationDir = 0.f;
        ecs::Signal<float> onRotate = {};
        ecs::Signal<> onBeginRotate = {};
        ecs::Signal<> onFinishRotate = {};

        std::shared_ptr<render::VertexBuffer> quarterVertBuffer = {};
        std::shared_ptr<render::IndexBuffer> quarterIndexBuffer = {};
        geo::AABB quarterAABB = {};

        std::shared_ptr<render::VertexBuffer> fullVertBuffer = {};
        std::shared_ptr<render::IndexBuffer> fullIndexBuffer = {};
        geo::AABB fullAABB = {};
    };
}
