#include "spark.h"

#include "WidgetVisibilitySystem.h"
#include "engine/profiling/Profiler.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void WidgetVisibilitySystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("WidgetVisibilitySystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& widget = widgets[i];
            if (widget.dirty)
            {
                UpdateWidgetVisibility(entities[i], widget);
            }
        }
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
                auto& childWidget = childWidgets[i];
                childWidget.parentVisibility = std::min(widget.visibility, widget.parentVisibility);
                childWidget.parentUpdateEnabled = widget.updateEnabled && widget.parentUpdateEnabled;
                UpdateWidgetVisibility(children[i], childWidget);
            }

            return false;
        });

        widget.dirty = false;
    }
}
