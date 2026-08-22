module;

#include "spark.h"
#include "easy/profiler.h"
#include "engine/ecs/SystemDeclaration.h"
#include "engine/ecs/UpdateMode.h"
#include "engine/ecs/World.h"
#include "engine/ecs/components/RootComponent.h"




module RootTransformSystem;
import SystemUtil;

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

            util::ParallelForEachEntity(updateData, [this, &entities, &transform](const size_t i)
            {
                const auto& entity = entities[i];
                auto& trans = transform[i];
                trans.worldTransform = trans.transform;

                const HeirachyQueryDeclaration dec = HeirachyQueryDeclaration()
                    .WithComponent<TransformComponent>();

                RunRecursiveChildQuery(entity, dec, [&trans](const SystemUpdateData& updateData)
                {
                    auto* childTransform = updateData.GetComponentArray<TransformComponent>();
                    childTransform->worldTransform = trans.worldTransform * childTransform->transform;
                    if (childTransform->parent != &trans) [[unlikely]]
                    {
                        childTransform->parent = &trans;
                    }
                    return false;
                });
            });
        });
    }
}
