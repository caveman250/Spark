#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::geo::components
{
    struct AABBColliderComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        AABB aabb;
    };
}
