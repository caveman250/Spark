#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "LastRectSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "engine/Application.h"
#include <easy/profiler.h>

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration LastRectSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("LastRectSystem")
                    .WithComponent<ui::components::RectTransformComponent>();
    }

    void LastRectSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("LastRectSystem::OnUpdate");

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& trans = transform[i];
            if (!trans.needsLayout) // some layout optimisations rely on a position delta.
            {
                trans.lastRect = trans.rect;
            }
        }
    }
}
