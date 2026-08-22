module;
#include "spark.h"

#include <engine/ui/util/RectTransformUtil.h>
#include <easy/profiler.h>

#include "engine/ecs/UpdateMode.h"

module LastRectSystem;
import Application;
import SystemUtil;
import MeshComponent;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration LastRectSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("LastRectSystem")
                    .WithComponent<ui::components::RectTransformComponent>();
    }

    void LastRectSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("LastRectSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [](const ecs::SystemUpdateData& updateData)
        {
            auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();
            for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
            {
                auto& trans = transform[i];
                if (!trans.needsLayout) // some layout optimisations rely on a position delta.
                {
                    trans.lastRect = trans.rect;
                }
            }
        });
    }
}
