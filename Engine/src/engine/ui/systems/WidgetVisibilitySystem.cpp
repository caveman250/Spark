#include "spark.h"

#include "WidgetVisibilitySystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/math/Mat4.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(WidgetVisibilitySystem)

    void WidgetVisibilitySystem::OnUpdate(const std::vector<ecs::Id>& entities, components::WidgetComponent* widgets)
    {
        PROFILE_SCOPE("WidgetVisibilitySystem::OnUpdate")

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& widget = widgets[i];
            if (widget.dirty)
            {
                RunChildQuery<components::WidgetComponent>(entities[i],
                [widget](const std::vector<ecs::Id>& children, components::WidgetComponent* childWidgets)
                {
                    for (size_t i = 0; i < children.size(); ++i)
                    {
                        auto& childWidget = childWidgets[i];
                        childWidget.parentRenderingEnabled = widget.renderingEnabled && widget.parentRenderingEnabled;
                        childWidget.dirty = true;
                    }
                });

                widget.dirty = false;
            }
        }
    }
}
