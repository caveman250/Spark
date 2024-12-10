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
    DEFINE_SPARK_SYSTEM(WorldTransformSystem)

    void WorldTransformSystem::OnUpdate(const std::vector<Id>& entities, TransformComponent* transform, ParentComponent*)
    {
        PROFILE_SCOPE("WorldTransformSystem::OnUpdate")

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const TransformComponent& parent = transform[i];
            RunChildQuery<TransformComponent>(entities[i], [parent](const std::vector<Id>& children, TransformComponent* childTransform)
            {
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
