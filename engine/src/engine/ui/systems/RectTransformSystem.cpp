#include "RectTransformSystem.h"

#include <easy/profiler.h>
#include "Widgets.generated.h"
#include "engine/Application.h"
#include "engine/ui/util/RectTransformUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration RectTransformSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("RectTransformSystem")
                    .WithComponent<ui::components::RectTransformComponent>()
                    .WithVariantComponent<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Mutable)
                    .WithHeirachyQuery<components::WidgetComponent>()
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

        std::visit([this, updateData, entities, world, transform](auto&& value)
        {
            ecs::util::ForEachEntity(this, updateData,
            [this, world, entities, transform, value](size_t i)
            {
                const auto& entity = entities[i];
                auto& trans = transform[i];

                if (trans.needsLayout && world->HasComponent<ParentComponent>(entity))
                {
                    Layout::LayoutWidgetChildren(world, this, entities[i], trans, trans.layer, value + i);
                    trans.needsLayout = false;
                }
            });
        }, updateData.GetVariantComponentArray<SPARK_WIDGET_TYPES_WITH_NULLTYPE>());
    }
}
