#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "RootTransformSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include <easy/profiler.h>

#include "engine/ecs/util/SystemUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    SystemDeclaration RootTransformSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("RootTransformSystem")
                .WithComponent<TransformComponent>()
                .WithComponent<const RootComponent>();
    }

    void RootTransformSystem::OnUpdate(const QueryResults& results)
    {
        EASY_BLOCK("RootTransformSystem::OnUpdate");

        ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto entities = updateData.GetEntities();
            auto* transform = updateData.GetComponentArray<TransformComponent>();

            util::ParallelForEachEntity(updateData, [this, &entities, &transform](size_t i)
            {
                const auto& entity = entities[i];
                auto& trans = transform[i];
                trans.worldTransform = trans.transform;

                HeirachyQueryDeclaration dec = HeirachyQueryDeclaration()
                    .WithComponent<TransformComponent>();

                RunRecursiveChildQuery(entity, dec, [trans](const SystemUpdateData& updateData)
                {
                    auto* childTransform = updateData.GetComponentArray<TransformComponent>();
                    childTransform->worldTransform = trans.worldTransform * childTransform->transform;
                    return false;
                });
            });
        });
    }
}
