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

    ecs::UpdateMode RectTransformSystem::GetUpdateMode() const
    {
        return ecs::UpdateMode::SingleThreaded;
    }

    void RectTransformSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("RectTransformSystem::OnUpdate");
        auto world = Application::Get()->GetWorld();

        const auto& entities = updateData.GetEntities();
        auto* transform = updateData.GetComponentArray<components::RectTransformComponent>();

        ecs::util::ForEachEntity(this, updateData,
        [this, world, entities, transform](size_t i)
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
        });
    }
}
