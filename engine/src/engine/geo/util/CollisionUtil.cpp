#include "CollisionUtil.h"
#include "engine/ecs/components/TransformComponent.h"

namespace se::geo::util
{
    std::optional<RayCastHit> RayCastAABB(const Ray& ray, const ecs::components::TransformComponent& transform)
    {
        const math::Vec3 origin = math::Inverse(transform.worldTransform) * math::Vec4(ray.origin, 1.0f);
        const math::Vec3 direction = math::Inverse(transform.worldTransform) * math::Vec4(ray.direction, 0.0f);
        const Ray localSpaceRay(origin, direction);

        const math::Vec3 min = (transform.aabb.pos - localSpaceRay.origin) / localSpaceRay.direction;
        const math::Vec3 max = (transform.aabb.pos + transform.aabb.size - localSpaceRay.origin) / localSpaceRay.direction;

        const math::Vec3 near = { std::min(min.x, max.x), std::min(min.y, max.y), std::min(min.z, max.z)};
        const math::Vec3 far = { std::max(min.x, max.x), std::max(min.y, max.y), std::max(min.z, max.z)};

        const float nearMax = std::max(std::max(near.x, near.y), near.z);
        const float farMin = std::min(std::min(far.x, far.y), far.z);

        if (nearMax <= farMin)
        {
            return RayCastHit(ray.origin + ray.direction * nearMax);
        }

        return std::nullopt;
    }

    std::optional<RayCastHit> RayCastPlane(const Ray& ray,
                       const Plane& plane)
    {
        const float numer = math::Dot(plane.normal, ray.origin) - plane.distSquared;
        const float denom = math::Dot(plane.normal, ray.direction);

        if (std::abs(denom) < FLT_EPSILON)
        {
            return std::nullopt;
        }

        const float dist = -(numer / denom);
        return RayCastHit(ray.origin + ray.direction * dist);
    }
}
