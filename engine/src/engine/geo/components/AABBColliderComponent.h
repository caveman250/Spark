#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/geo/AABB.h"
#include "engine/ecs/Signal.h"
#include "engine/geo/singleton_components/CollisionComponent.h"

namespace se::geo::components
{
    struct AABBColliderComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        AABB aabb;

        SPARK_MEMBER(Serialized)
        bool dynamic;

        SPARK_MEMBER(Serialized)
        ecs::Signal<ecs::Id> onCollisionEnter;

        SPARK_MEMBER(Serialized)
        singleton_components::CollisionChannel collisionChannel = singleton_components::CollisionChannel::Default;
    };
}
