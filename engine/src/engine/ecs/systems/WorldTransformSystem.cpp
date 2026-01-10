#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "WorldTransformSystem.h"

#include "RootTransformSystem.h"
#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include <easy/profiler.h>

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    SystemDeclaration WorldTransformSystem::GetSystemDeclaration()
    {
        return SystemDeclaration("WorldTransformSystem")
                    .WithComponent<TransformComponent>()
                    .WithComponent<ParentComponent>()
                    .WithDependency<RootTransformSystem>();
    }

    void WorldTransformSystem::OnUpdate(const SystemUpdateData& updateData)
    {
        EASY_BLOCK("WorldTransformSystem::OnUpdate");

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<TransformComponent>();

        util::ForEachEntity(this, updateData, [this, entities, transform](size_t i)
        {
            const TransformComponent& parent = transform[i];
            auto declaration = HeirachyQueryDeclaration()
                                .WithComponent<TransformComponent>();
            RunChildQuery(entities[i], declaration,
              [parent](const SystemUpdateData& updateData)
            {
                const auto& children = updateData.GetEntities();
                auto* childTransform = updateData.GetComponentArray<TransformComponent>();
                for (size_t i = 0; i < children.size(); ++i)
                {
                    TransformComponent& transform = childTransform[i];
                    transform.worldTransform = parent.worldTransform * transform.transform;
                }

                return false;
            });
        });
    }
}
