#include "AABBColliderSystem.h"

#include "ResetCollisionSystem.h"
#include "engine/render/Renderer.h"
#include "engine/render/components/DirLightComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/systems/TransformSystem.h"
#include "engine/geo/components/AABBColliderComponent.h"
#include "engine/geo/singleton_components/CollisionComponent.h"

namespace se::geo::systems
{
    ecs::SystemDeclaration AABBColliderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("AABBColliderSystem")
                .WithComponent<components::AABBColliderComponent>()
                .WithComponent<const ecs::components::TransformComponent>()
                .WithSingletonComponent<singleton_components::CollisionComponent>()
                .WithDependency<ResetCollisionSystem>()
                .WithDependency<ecs::systems::TransformSystem>();
    }

    void AABBColliderSystem::OnUpdate(const ecs::QueryResults& results)
    {
#if SPARK_EDITOR
        auto* editor = Application::Get()->GetEditor();
#endif

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [editor](const ecs::SystemUpdateData& updateData)
        {
            const auto* transforms = updateData.GetComponentArray<const ecs::components::TransformComponent>();
            const auto* colliders = updateData.GetComponentArray<components::AABBColliderComponent>();
            auto* collisionComp = updateData.GetSingletonComponent<singleton_components::CollisionComponent>();
            const auto& entities = updateData.GetEntities();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                const auto& transform = transforms[i];
                const auto& collider = colliders[i];

                collisionComp->mutex.lock();
                collisionComp->colliders.insert(std::make_pair(entity, singleton_components::ColliderRecord{ collider.dynamic, collider.aabb, transform.pos }));
                collisionComp->mutex.unlock();
#if SPARK_EDITOR
                if (editor->InGameMode())
                {
                    continue;
                }

                if (editor->GetSelectedEntity() == entity)
                {
                    auto& dg = debug::Graphics::Get();
                    const math::Vec4 colour = math::Vec4(0.f, 1.f, 0.5f, 1.f);
                    math::Vec3 t1 = transform.pos + collider.aabb.center - collider.aabb.size;
                    math::Vec3 t2 = t1 + math::Vec3(collider.aabb.size.x * 2, 0.f, 0.f);
                    math::Vec3 t3 = t1 + math::Vec3(collider.aabb.size.x * 2, 0.f, collider.aabb.size.z * 2);
                    math::Vec3 t4 = t1 + math::Vec3(0.f, 0.f, collider.aabb.size.z * 2);

                    math::Vec3 b1 = t1 + math::Vec3(0.f, collider.aabb.size.y * 2, 0.f);
                    math::Vec3 b2 = b1 + math::Vec3(collider.aabb.size.x * 2, 0.f, 0.f);
                    math::Vec3 b3 = b1 + math::Vec3(collider.aabb.size.x * 2, 0.f, collider.aabb.size.z * 2);
                    math::Vec3 b4 = b1 + math::Vec3(0.f, 0.f, collider.aabb.size.z * 2);

                    dg.AddLine(t1, t2, colour);
                    dg.AddLine(t1, t4, colour);
                    dg.AddLine(t3, t2, colour);
                    dg.AddLine(t3, t4, colour);

                    dg.AddLine(b1, b2, colour);
                    dg.AddLine(b1, b4, colour);
                    dg.AddLine(b3, b2, colour);
                    dg.AddLine(b3, b4, colour);

                    dg.AddLine(t1, b1, colour);
                    dg.AddLine(t2, b2, colour);
                    dg.AddLine(t3, b3, colour);
                    dg.AddLine(t4, b4, colour);

                }
#endif
            }
        });
    }
}
