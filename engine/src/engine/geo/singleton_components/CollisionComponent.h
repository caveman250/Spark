#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/geo/AABB.h"

namespace se::geo::singleton_components
{
    struct ColliderRecord
    {
        bool dynamic = false;
        AABB aabb = {};
        math::Vec3 pos = {};
    };
    struct CollisionComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT()

        std::map<ecs::Id, ColliderRecord> colliders = {};
        std::mutex mutex = {};
    };
}
