#include "AABBColliderResolveSystem.h"

#include "CollisionSystem.h"
#include "ResetCollisionSystem.h"
#include "engine/render/Renderer.h"
#include "engine/render/components/DirLightComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/Signal.h"
#include "engine/geo/components/AABBColliderComponent.h"
#include "engine/geo/singleton_components/CollisionComponent.h"

namespace se::geo::systems
{
    ecs::SystemDeclaration AABBColliderResolveSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("AABBColliderResolveSystem")
                .WithComponent<components::AABBColliderComponent>()
                .WithComponent<ecs::components::TransformComponent>()
                .WithSingletonComponent<singleton_components::CollisionComponent>()
                .WithDependency<CollisionSystem>();
    }

    void AABBColliderResolveSystem::OnUpdate(const ecs::QueryResults& results)
    {
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [](const ecs::SystemUpdateData& updateData)
        {
            auto* aabbs = updateData.GetComponentArray<components::AABBColliderComponent>();
            auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
            auto* collisionComp = updateData.GetSingletonComponent<singleton_components::CollisionComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                auto& aabb = aabbs[i];
                auto& transform = transforms[i];

                const auto& collider = collisionComp->colliders.at(entity);
                transform.pos = collider.pos;
                if (collider.collision)
                {
                    aabb.onCollisionEnter.Broadcast(entity);
                }
            }
        });
    }
}
