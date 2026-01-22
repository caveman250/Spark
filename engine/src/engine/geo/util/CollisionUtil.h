#pragma once

#include "spark.h"
#include "engine/geo/Plane.h"
#include "engine/geo/Ray.h"

namespace se::ecs::components
{
    struct TransformComponent;
}

namespace se::geo::util
{
    struct RayCastHit
    {
        math::Vec3 intersectionPoint = {};
    };

    std::optional<RayCastHit> RayCastAABB(const Ray& ray, const ecs::components::TransformComponent& transform);
    std::optional<RayCastHit> RayCastPlane(const Ray& ray, const Plane& plane);
}
