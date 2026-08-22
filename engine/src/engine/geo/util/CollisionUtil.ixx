module;

#include "spark.h"

export module Spark.Geo.Util.CollisionUtil;
import Spark.ECS.Components.TransformComponent;
import Spark.Geo.Ray;
import Spark.Geo.Plane;
import Spark.Math;

namespace se::geo::util
{
    struct RayCastHit
    {
        math::Vec3 intersectionPoint = {};
    };

    std::optional<RayCastHit> RayCastAABB(const Ray& ray, const ecs::components::TransformComponent& transform);
    std::optional<RayCastHit> RayCastPlane(const Ray& ray, const Plane& plane);
}
