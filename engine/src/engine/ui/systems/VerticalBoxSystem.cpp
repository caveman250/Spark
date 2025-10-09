#include "spark.h"

#include "VerticalBoxSystem.h"
#include "engine/profiling/Profiler.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/RectTransformUtil.h"
#include "engine/ui/systems/RectTransformSystem.h"
#include "engine/ui/systems/ScrollViewUpdateSystem.h"
#include "Widgets.generated.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration VerticalBoxSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Vertical Box System")
                    .WithComponent<ui::components::VerticalBoxComponent>()
                    .WithComponent<ui::components::RectTransformComponent>()
                    .WithComponent<const ui::components::WidgetComponent>()
                    .WithDependency<RectTransformSystem>()
                    .WithDependency<ScrollViewUpdateSystem>()
                    .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
    }

    void VerticalBoxSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("VerticalBoxSystem::OnUpdate")

        auto world = Application::Get()->GetWorld();
        const auto& entities = updateData.GetEntities();
        auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
        auto* verticalBoxes = updateData.GetComponentArray<components::VerticalBoxComponent>();
        const auto* widgets = updateData.GetComponentArray<const components::WidgetComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& entity = entities[i];
            auto& verticalBoxTransform = rectTransforms[i];
            auto& verticalBox = verticalBoxes[i];
            const auto& widget = widgets[i];

            if (!widget.updateEnabled || !widget.parentUpdateEnabled)
            {
                continue;
            }

            if (verticalBoxTransform.needsLayout)
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

                auto childRects = util::GetChildrenDesiredSizes(entity, this, verticalBoxTransform);
                auto window = Application::Get()->GetWindow();
                float currY = verticalBox.paddingTop * window->GetContentScale();
                for (const auto& child: world->GetChildren(entity))
                {
                    auto desiredSizeInfo = childRects.at(child);
                    desiredSizeInfo.rectTransform->rect.topLeft = verticalBoxTransform.rect.topLeft + math::Vec2 { verticalBox.paddingLeft * window->GetContentScale(), currY };
                    desiredSizeInfo.rectTransform->rect.size = desiredSizeInfo.desiredSize;
                    desiredSizeInfo.rectTransform->rect.size.x = verticalBoxTransform.rect.size.x -
                            verticalBox.paddingRight * window->GetContentScale() -
                            verticalBox.paddingLeft * window->GetContentScale();
                    currY += desiredSizeInfo.desiredSize.y + verticalBox.spacing;

                    desiredSizeInfo.rectTransform->layer = verticalBoxTransform.layer;

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

                verticalBoxTransform.rect.size.y = currY + verticalBox.paddingBottom * window->GetContentScale();
                verticalBoxTransform.maxY = verticalBoxTransform.minY + currY / window->GetContentScale() + verticalBox.paddingBottom;

                verticalBoxTransform.needsLayout = false;
            }
        }
    }
}
