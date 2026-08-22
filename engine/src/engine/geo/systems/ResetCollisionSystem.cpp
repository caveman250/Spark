module;


#include "engine/geo/singleton_components/CollisionComponent.h"

module ResetCollisionSystem;

namespace se::geo::systems
{
    ecs::SystemDeclaration ResetCollisionSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("ResetCollisionSystem")
                .WithSingletonComponent<singleton_components::CollisionComponent>();
    }

    void ResetCollisionSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto* collision = results.updateData.GetSingletonComponent<singleton_components::CollisionComponent>();
        collision->colliders.clear();
    }
}
