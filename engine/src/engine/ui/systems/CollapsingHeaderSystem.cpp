module;
#include "spark.h"
#include <easy/profiler.h>
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

#include "engine/threads/ParallelForEach.h"

module CollapsingHeaderSystem;
import SystemUtil;
import RectTransformSystem;
import Application;
import CollapsingHeaderComponent;
import TextComponent;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration CollapsingHeaderSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Collapsing Header System")
                .WithComponent<components::CollapsingHeaderComponent>()
                .WithComponent<components::RectTransformComponent>()
                .WithComponent<const components::MouseInputComponent>()
                .WithHeirachyQuery<components::WidgetComponent>()
                .WithDependency<RectTransformSystem>();
    }

    void CollapsingHeaderSystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("CollapsingHeaderSystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* collapsingHeaders = updateData.GetComponentArray<components::CollapsingHeaderComponent>();
            auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& entity = entities[i];
                auto& collapsingHeader = collapsingHeaders[i];
                auto& transform = transforms[i];

                if (collapsingHeader.collapsed != collapsingHeader.lastCollapsed)
                {
                    collapsingHeader.lastCollapsed = collapsingHeader.collapsed;
                    collapsingHeader.onCollapsed.Broadcast(collapsingHeader.collapsed);
                    transform.desiredSize = { };
                    transform.cachedParentSize = { };

                    auto declaration = ecs::HeirachyQueryDeclaration()
                            .WithComponent<components::WidgetComponent>();
                    RunQueryOnChild(collapsingHeader.contentsEntity, declaration,
                                    [&collapsingHeader](const ecs::SystemUpdateData& updateData)
                                    {
                                        auto* widget = updateData.GetComponentArray<components::WidgetComponent>();
                                        widget->visibility = collapsingHeader.collapsed ? Visibility::Collapsed : Visibility::Visible;
                                        widget->updateEnabled = !collapsingHeader.collapsed;
                                        widget->dirty = true;
                                    });

                    util::InvalidateParent(entity, this);
                }
            }
        });
    }
}
