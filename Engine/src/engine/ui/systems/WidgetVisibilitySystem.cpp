#include "spark.h"

#include "WidgetVisibilitySystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/math/Mat4.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(WidgetVisibilitySystem)

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
                auto declaration = ecs::ChildQueryDeclaration()
                    .WithComponent<components::WidgetComponent>();
                RunChildQuery(entities[i], declaration,
                [widget](const ecs::SystemUpdateData& updateData)
                {
                    const auto& children = updateData.GetEntities();
                    auto* childWidgets = updateData.GetComponentArray<components::WidgetComponent>();
                    for (size_t i = 0; i < children.size(); ++i)
                    {
                        auto& childWidget = childWidgets[i];
                        childWidget.parentRenderingEnabled = widget.renderingEnabled && widget.parentRenderingEnabled;
                        childWidget.dirty = true;
                    }

                    return false;
                });

                widget.dirty = false;
            }
        }
    }
}
