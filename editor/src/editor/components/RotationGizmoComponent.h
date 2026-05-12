#pragma once 

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/ecs/Signal.h"
#include "engine/geo/AABB.h"
#include "engine/math/math.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/VertexBuffer.h"

namespace se::editor::components 
{
    SPARK_ENUM()
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
        math::Vec4 selectedColor = {};
        RotationAxis axis = RotationAxis::X;
        math::Vec3 initialClickPos = {};
        bool wasHovered = true;
        bool wasMouseDown = false;
        bool mouseDown = false;
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