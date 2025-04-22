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

    void RectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("RectTransformSystem::OnUpdate")
        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<ui::components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            trans.lastRect = trans.rect;
            if (trans.needsLayout)
            {
                if (!trans.overridesChildSizes)
                {
                    util::LayoutChildren(world, this, entities[i], trans, trans.layer + 1);
                    trans.needsLayout = false;
                }
            }
        }
    }

    void RectTransformSystem::LayoutChildren(ecs::World* world, ecs::Id entity, const components::RectTransformComponent &parentRect, int depth)
    {
        auto dec = ecs::ChildQueryDeclaration()
            .WithComponent<components::RectTransformComponent>();
        RunChildQuery(entity, dec,
        [this, world, parentRect, depth](const ecs::SystemUpdateData& updateData)
        {
            const auto& children = updateData.GetEntities();
            auto* childTransform = updateData.GetComponentArray<components::RectTransformComponent>();

            for (size_t i = 0; i < children.size(); ++i)
            {
                components::RectTransformComponent& child = childTransform[i];
                child.lastRect = child.rect;
                child.rect = util::CalculateScreenSpaceRect(child, parentRect);
                child.layer = depth;

                if (world->HasComponent<ParentComponent>(children[i]) && !child.overridesChildSizes)
                {
                    LayoutChildren(world, children[i], child, depth + 1);
                }
            }

            return false;
        });
    }
}
