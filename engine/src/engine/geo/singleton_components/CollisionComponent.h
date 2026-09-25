#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/geo/AABB.h"

namespace se::geo::singleton_components
{
    SPARK_ENUM()
    enum class CollisionChannel
    {
        Default,
        Player,
        Bullet,
        COUNT
    };

    struct ColliderRecord
    {
        bool dynamic = false;
        AABB aabb = {};
        math::Vec3 pos = {};
        CollisionChannel channel = {};
        bool collision = false;
    };

    struct CollisionComponent : ecs::Component
    {
        SPARK_SINGLETON_COMPONENT()

        std::map<ecs::Id, ColliderRecord> colliders = {};
        static constexpr size_t NumChannels = static_cast<size_t>(CollisionChannel::COUNT);
        std::array<std::array<bool, NumChannels>, NumChannels> collisionChannels =
        {
            std::array{ true, true, true },
            std::array{ true, true, false },
            std::array{ true, false, true }
        };
        std::mutex mutex = {};
    };
}
