#include "spark.h"

#include "engine/math/Mat4.h"
#include "RectTransformSystem.h"

#include <engine/ui/util/RectTransformUtil.h>

#include "RootRectTransformSystem.h"
#include "engine/Application.h"
#include <easy/profiler.h>

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration RectTransformSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("RectTransformSystem")
                    .WithComponent<ui::components::RectTransformComponent>()
                    .WithDependency<RootRectTransformSystem>();
    }

    void RectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("RectTransformSystem::OnUpdate");
        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& trans = transform[i];

            if (trans.needsLayout && world->HasComponent<ParentComponent>(entity))
            {
                if (!trans.overridesChildSizes)
                {
                    util::LayoutChildren(world, this, entities[i], trans, trans.layer);
                    trans.needsLayout = false;
                }
            }
        }
    }
}
