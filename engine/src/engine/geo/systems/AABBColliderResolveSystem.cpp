module;


#include "engine/render/components/DirLightComponent.h"


#include "engine/geo/components/AABBColliderComponent.h"
#include "engine/geo/singleton_components/CollisionComponent.h"

module AABBColliderResolveSystem;
import ResetCollisionSystem;
import CollisionSystem;

namespace se::geo::systems
{
    ecs::SystemDeclaration AABBColliderResolveSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("AABBColliderResolveSystem")
                .WithComponent<const components::AABBColliderComponent>()
                .WithComponent<ecs::components::TransformComponent>()
                .WithSingletonComponent<singleton_components::CollisionComponent>()
                .WithDependency<CollisionSystem>();
    }

    void AABBColliderResolveSystem::OnUpdate(const ecs::QueryResults& results)
    {
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [](const ecs::SystemUpdateData& updateData)
        {
            auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();
            auto* collisionComp = updateData.GetSingletonComponent<singleton_components::CollisionComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                auto& transform = transforms[i];

                transform.pos = collisionComp->colliders.at(entity).pos;
            }
        });
    }
}
