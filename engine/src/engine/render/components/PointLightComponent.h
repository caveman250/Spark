#pragma once

#include "spark.h"
#include "engine/math/math.h"
#include "engine/ecs/Component.h"

namespace se::render::components
{
    struct PointLightComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        math::Vec3 color;

#if SPARK_EDITOR
        std::shared_ptr<MaterialInstance> iconMaterial = {};
        std::shared_ptr<VertexBuffer> vertBuffer = {};
        std::shared_ptr<IndexBuffer> indexBuffer = {};
#endif
    };
}
