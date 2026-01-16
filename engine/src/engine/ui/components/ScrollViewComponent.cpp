#include "ScrollViewComponent.h"

#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    template<>
    void Layout::LayoutWidgetChildren<components::ScrollViewComponent>(ecs::World* world,
                                                                       ecs::System* system,
                                                                       const ecs::Id& entity,
                                                                       components::RectTransformComponent& rectTransform,
                                                                       int layer,
                                                                       components::ScrollViewComponent* scrollView)
    {
        auto window = Application::Get()->GetWindow();
        auto declaration = ecs::HeirachyQueryDeclaration()
                .WithComponent<components::RectTransformComponent>()
                .WithComponent<components::WidgetComponent>();
        float totalChildSize = 0.f;
        util::ForEachWidgetChild(entity, system, declaration,
        [system, world, window, &rectTransform, &totalChildSize, layer](const ecs::SystemUpdateData& updateData, auto&& widgetComps)
        {
            const auto& children = updateData.GetEntities();
            auto* childTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
            auto widgets = updateData.GetComponentArray<components::WidgetComponent>();
            for (size_t i = 0; i < children.size(); ++i)
            {
                const auto& child = children[i];
                auto& childTransform = childTransforms[i];
                auto oldTransform = childTransform;
                childTransform.anchors = { 0, 1, 0, 0 };
                auto desiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, &widgets[i], rectTransform, childTransform, &widgetComps[i]);
                childTransform.maxY = childTransform.minY + desiredSize.y / window->GetContentScale();
                childTransform.rect = util::CalculateScreenSpaceRect(childTransform, rectTransform);
                totalChildSize += childTransform.rect.size.y;
                LayoutWidgetChildren(world, system, child, rectTransform, layer, &widgetComps[i]);
                childTransform.needsLayout = false;
            }

            return false;
        });

        float availableScrollSpaceBottom = std::max(0.f, totalChildSize - rectTransform.rect.size.y);
        if (availableScrollSpaceBottom > 0.f)
        {
            util::TranslateChildren(entity, system, math::IntVec2(0, static_cast<int>(-availableScrollSpaceBottom * scrollView->scrollAmount)));
            scrollView->onScrolled.Broadcast(&rectTransform, scrollView->scrollAmount);
        }

        rectTransform.needsLayout = false;
    }
}
