#include "CollisionSystem.h"

#include "AABBColliderSystem.h"
#include "engine/render/Renderer.h"
#include "engine/geo/singleton_components/CollisionComponent.h"

namespace se::geo::systems
{
    ecs::SystemDeclaration CollisionSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("CollisionSystem")
                .WithSingletonComponent<singleton_components::CollisionComponent>()
                .WithDependency<AABBColliderSystem>();
    }

    bool Overlaps(const singleton_components::ColliderRecord& a, const singleton_components::ColliderRecord& b)
    {
        math::Vec3 tlA = a.pos + a.aabb.center - a.aabb.size;
        math::Vec3 tlB = b.pos + b.aabb.center - b.aabb.size;
        math::Vec3 brA = a.pos + a.aabb.center + a.aabb.size;
        math::Vec3 brB = b.pos + b.aabb.center + b.aabb.size;

        return tlB.x <= brA.x && brB.x >= tlA.x &&
                tlB.y <= brA.y && brB.y >= tlA.y &&
                tlB.z <= brA.z && brB.z >= tlA.z;
    }

    void CollisionSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto* collision = results.updateData.GetSingletonComponent<singleton_components::CollisionComponent>();

        for (auto& record : collision->colliders)
        {
            if (record.second.dynamic)
            {
                for (const auto& otherRecord : collision->colliders)
                {
                    if (otherRecord.first == record.first)
                    {
                        continue;
                    }

                    if (Overlaps(record.second, otherRecord.second))
                    {
                        math::Vec3 delta = otherRecord.second.pos - record.second.pos;
                        float overlapX = record.second.aabb.size.x + otherRecord.second.aabb.size.x - std::abs(delta.x);
                        float overlapY = record.second.aabb.size.y + otherRecord.second.aabb.size.y - std::abs(delta.y);
                        float overlapZ = record.second.aabb.size.z + otherRecord.second.aabb.size.z - std::abs(delta.z);

                        if (overlapX < overlapY && overlapX < overlapZ)
                        {
                            record.second.pos.x += delta.x > 0.f ? -overlapX : overlapX;
                        }
                        else if (overlapY < overlapX && overlapY < overlapZ)
                        {
                            record.second.pos.y += delta.y > 0.f ? -overlapY : overlapY;
                        }
                        else
                        {
                            record.second.pos.z += delta.z > 0.f ? -overlapZ : overlapZ;
                        }
                    }
                }
            }
        }
    }
}
