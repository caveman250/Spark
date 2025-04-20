#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "RootTransformSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/relationships/ChildOf.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    DEFINE_SPARK_SYSTEM(RootTransformSystem)

    void RootTransformSystem::OnUpdate(const SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("RootTransformSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<TransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            trans.worldTransform = trans.transform;
        }
    }
}
