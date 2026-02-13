#include "spark.h"

#include "WidgetVisibilitySystem.h"
#include <easy/profiler.h>

#include "engine/ecs/util/SystemUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration WidgetVisibilitySystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("WidgetVisibilitySystem")
                .WithComponent<components::WidgetComponent>();
    }

    void WidgetVisibilitySystem::OnUpdate(const ecs::QueryResults& results)
    {
        EASY_BLOCK("WidgetVisibilitySystem::OnUpdate");

        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [this](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();
            for (size_t i = 0; i < entities.size(); ++i)
            {
                [[maybe_unused]] const auto& entity = entities[i];
                auto& widget = widgets[i];
                if (widget.dirty)
                {
                    UpdateWidgetVisibility(entities[i], widget);
                }
            }
        });
    }

    void WidgetVisibilitySystem::UpdateWidgetVisibility(const ecs::Id& entity,
        components::WidgetComponent& widget)
    {
        auto declaration = ecs::HeirachyQueryDeclaration()
                    .WithComponent<components::WidgetComponent>();
        RunChildQuery(entity, declaration,
        [this, widget](const ecs::SystemUpdateData& updateData)
        {
            const auto& children = updateData.GetEntities();
            auto* childWidgets = updateData.GetComponentArray<components::WidgetComponent>();
            for (size_t i = 0; i < children.size(); ++i)
            {
                [[maybe_unused]] const auto& child = children[i];
                auto& childWidget = childWidgets[i];
                {
                    childWidget.parentVisibility = std::min(widget.visibility, widget.parentVisibility);
                    childWidget.parentUpdateEnabled = widget.updateEnabled && widget.parentUpdateEnabled;
                }

                UpdateWidgetVisibility(children[i], childWidget);
            }

            return false;
        });

        widget.dirty = false;
    }
}
