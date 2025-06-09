#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "WorldTransformSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    void WorldTransformSystem::OnUpdate(const SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("WorldTransformSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<TransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
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
        }
    }
}
