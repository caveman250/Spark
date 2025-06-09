#include "spark.h"
#include "CollapsingHeaderSystem.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/components/CollapsingHeaderComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/math/IntVec2.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void CollapsingHeaderSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("CollapsingHeaderSystem::OxnUpdate")

        auto world = Application::Get()->GetWorld();

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
                // transform.maxY =
                //     DesiredSizeCalculator::GetDesiredSize(this, entity, transform, &collapsingHeader).y /
                //         window->GetContentScale();
                // transform.rect.size.y = transform.maxY * window->GetContentScale();
                collapsingHeader.lastCollapsed = collapsingHeader.collapsed;
                collapsingHeader.onCollapsed.Broadcast(collapsingHeader.collapsed);

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
                continue;
            }

            if (transform.needsLayout)
            {
                auto desiredSizeInfo = util::GetChildrenDesiredSizes(entity, this, transform);
                for (const auto& [child, desired] : desiredSizeInfo)
                {
                    if (child == collapsingHeader.contentsEntity && collapsingHeader.collapsed)
                    {
                        continue;
                    }

                    if (child == collapsingHeader.titleEntity)
                    {
                        desired.rectTransform->rect.topLeft = transform.rect.topLeft;
                        desired.rectTransform->rect.size.x = transform.rect.size.x;
                        desired.rectTransform->rect.size.y = desired.desiredSize.y;
                    }
                    else if (child == collapsingHeader.contentsEntity)
                    {
                        const auto& titleDesired = desiredSizeInfo[collapsingHeader.titleEntity];
                        int titleMaxY = titleDesired.desiredSize.y;

                        desired.rectTransform->rect.topLeft = transform.rect.topLeft + math::Vec2(0.f, titleMaxY + 5.f);
                        desired.rectTransform->rect.size.x = transform.rect.size.x;
                        desired.rectTransform->rect.size.y = desired.desiredSize.y;
                        {
                            util::GetChildrenDesiredSizes(entity, this, transform);
                        }
                    }
                    else
                    {
                        SPARK_ASSERT(false, "Unexpected CollapsingHeader child.");
                    }

                    if (!desired.rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world, this, child, *desired.rectTransform, desired.rectTransform->layer + 1);
                        desired.rectTransform->needsLayout = false;
                    }
                    else
                    {
                        desired.rectTransform->needsLayout = true;
                    }
                }

                transform.needsLayout = false;
            }
        }
    }
}
