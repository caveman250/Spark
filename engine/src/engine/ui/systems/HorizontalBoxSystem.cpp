#include "spark.h"

#include "HorizontalBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/components/HorizontalBoxComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/RectTransformUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    void HorizontalBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("HorizontalBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
        auto* horizontalBoxes = updateData.GetComponentArray<components::HorizontalBoxComponent>();
        const auto* widgets = updateData.GetComponentArray<const components::WidgetComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& horizontalBoxTransform = rectTransforms[i];
            auto& horizontalBox = horizontalBoxes[i];
            const auto& widget = widgets[i];

            if (!widget.updateEnabled || !widget.parentUpdateEnabled)
            {
                continue;
            }

            if (horizontalBoxTransform.needsLayout)
            {
                // make sure our anchors are set correctly so that desired size works as expected.
                auto dec = ecs::HeirachyQueryDeclaration()
                        .WithComponent<components::RectTransformComponent>();
                RunChildQuery(entity, dec,
                [](const ecs::SystemUpdateData& updateData)
                {
                    const auto& children = updateData.GetEntities();
                    auto* rects = updateData.GetComponentArray<components::RectTransformComponent>();

                    for (size_t i = 0; i < children.size(); ++i)
                    {
                        auto& rect = rects[i];
                        rect.anchors = { 0.f, 1.f, 0.f, 0.f };
                        rect.minY = 0;
                        rect.maxY = 0;
                        rect.minX = 0;
                        rect.maxX = 0;
                    }

                    return false;
                });

                auto childRects = util::GetChildrenDesiredSizes(entity, this, horizontalBoxTransform);
                auto window = Application::Get()->GetWindow();
                float currX = horizontalBox.paddingLeft * window->GetContentScale();
                for (const auto& child: world->GetChildren(entity))
                {
                    auto desiredSizeInfo = childRects.at(child);
                    desiredSizeInfo.rectTransform->rect.topLeft = horizontalBoxTransform.rect.topLeft + math::Vec2 { currX, horizontalBox.paddingTop };
                    desiredSizeInfo.rectTransform->rect.size = desiredSizeInfo.desiredSize;
                    desiredSizeInfo.rectTransform->rect.size.y = horizontalBoxTransform.rect.size.y - horizontalBox.paddingBottom - horizontalBox.paddingTop;
                    currX += desiredSizeInfo.desiredSize.x + horizontalBox.spacing;

                    desiredSizeInfo.rectTransform->layer = horizontalBoxTransform.layer;

                    if (!desiredSizeInfo.rectTransform->overridesChildSizes)
                    {
                        util::LayoutChildren(world,
                                             this,
                                             child,
                                             *desiredSizeInfo.rectTransform,
                                             desiredSizeInfo.rectTransform->layer);
                        desiredSizeInfo.rectTransform->needsLayout = false;
                    }
                    else
                    {
                        desiredSizeInfo.rectTransform->needsLayout = true;
                    }
                }

                horizontalBoxTransform.rect.size.x = currX + horizontalBox.paddingRight * window->GetContentScale();
                horizontalBoxTransform.maxX = horizontalBoxTransform.minX + currX / window->GetContentScale() + horizontalBox.paddingRight;

                horizontalBoxTransform.needsLayout = false;
            }
        }
    }
}
