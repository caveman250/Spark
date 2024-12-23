#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "RectTransformSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(RectTransformSystem)

    void RectTransformSystem::OnUpdate(const std::vector<ecs::Id>& entities, components::RectTransformComponent* transform, ParentComponent*)
    {
        PROFILE_SCOPE("RectTransformSystem::OnUpdate")
        auto world = Application::Get()->GetWorld();
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            if (trans.needsLayout)
            {
                LayoutChildren(world, entities[i], trans, trans.layer + 1);
                trans.needsLayout = false;
            }
        }
    }

    void RectTransformSystem::LayoutChildren(ecs::World* world, ecs::Id entity, const components::RectTransformComponent &parentRect, int depth)
    {
        RunChildQuery<components::RectTransformComponent>(entity, [this, world, parentRect, depth](const std::vector<ecs::Id>& children, components::RectTransformComponent* childTransform)
        {
            for (size_t i = 0; i < children.size(); ++i)
            {
                components::RectTransformComponent& child = childTransform[i];
                child.rect = util::CalculateScreenSpaceRect(child, parentRect);

                child.layer = depth;
                child.lastRect = child.rect;

                if (world->HasComponent<ParentComponent>(children[i]))
                {
                    LayoutChildren(world, children[i], child, depth + 1);
                }
            }

            return false;
        });
    }
}
