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

    void RootTransformSystem::OnUpdate(const SystemUpdateData& updateData)
    {
        EASY_BLOCK("RootTransformSystem::OnUpdate");

        auto* transform = updateData.GetComponentArray<TransformComponent>();

        util::ForEachEntity(this, updateData, [transform](size_t i)
        {
            auto& trans = transform[i];
            trans.worldTransform = trans.transform;
        });
    }
}
