#include "spark.h"
#include "CollapsingHeaderSystem.h"
#include "engine/Application.h"
#include "engine/input/InputUtil.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/components/CollapsingHeaderComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void CollapsingHeaderSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("CollapsingHeaderSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* collapsingHeaders = updateData.GetComponentArray<components::CollapsingHeaderComponent>();
        auto* transforms = updateData.GetComponentArray<components::RectTransformComponent>();
        //const auto* textComps = updateData.GetComponentArray<components::TextComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& collapsingHeader = collapsingHeaders[i];
            auto& transform = transforms[i];

            if (collapsingHeader.collapsed != collapsingHeader.lastCollapsed)
            {
                transform.maxY = DesiredSizeCalculator::GetDesiredSize(this, entity, transform, transform, &collapsingHeader).y;
                collapsingHeader.lastCollapsed = collapsingHeader.collapsed;
                collapsingHeader.onCollapsed.Broadcast(collapsingHeader.collapsed);

                auto declaration = ecs::ChildQueryDeclaration()
                            .WithComponent<components::WidgetComponent>();
                RunRecursiveChildQuery(entity, declaration,
                    [&collapsingHeader](const ecs::SystemUpdateData& updateData)
                    {
                        auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();
                        const auto& children = updateData.GetEntities();
                        for (size_t i = 0; i < children.size(); ++i)
                        {
                            auto& widget = widgets[i];
                            widget.renderingEnabled = !collapsingHeader.collapsed;
                        }

                        return false;
                    });
            }
        }
    }
}
