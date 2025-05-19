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
    void RectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("RectTransformSystem::OnUpdate")
        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<ui::components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& trans = transform[i];

            if (trans.needsLayout && world->HasComponent<ParentComponent>(entity))
            {
                if (!trans.overridesChildSizes)
                {
                    util::LayoutChildren(world, this, entities[i], trans, trans.layer + 1);
                    trans.needsLayout = false;
                }
            }
        }
    }
}
