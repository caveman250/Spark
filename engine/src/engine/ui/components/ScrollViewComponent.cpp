#include "ScrollViewComponent.h"

#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    template<>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::ScrollViewComponent>(ecs::System*,
                                                                   const ecs::Id&,
                                                                   components::RectTransformComponent& thisRect,
                                                                   const components::ScrollViewComponent*)
    {
        EASY_BLOCK("GetDesiredSize - ScrollViewComponent");
        auto window = Application::Get()->GetWindow();
        math::IntVec2 rectSize = GetDesiredSizeFromRect(thisRect);
        auto ret = math::IntVec2(std::max(rectSize.x, static_cast<int>(thisRect.minWidth * window->GetContentScale())),
                             std::max(rectSize.y, static_cast<int>(thisRect.minHeight * window->GetContentScale())));
        if (thisRect.maxWidth > 0)
        {
            ret.x = std::min(static_cast<int>(thisRect.maxWidth), ret.x);
        }
        if (thisRect.maxHeight > 0)
        {
            ret.x = std::min(static_cast<int>(thisRect.maxHeight), ret.x);
        }
        return ret;
    }

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
        [system, world, window, &rectTransform, &totalChildSize, layer](const ecs::SystemUpdateData& updateData, auto&& widgetComp)
        {
            const auto& child = updateData.GetEntity();
            auto* childTransform = updateData.GetComponentArray<components::RectTransformComponent>();
            auto widget = updateData.GetComponentArray<components::WidgetComponent>();
            childTransform->anchors = { 0, 1, 0, 0 };
            auto desiredSize = DesiredSizeCalculator::GetDesiredSize(system, child, widget, rectTransform, *childTransform, widgetComp);
            childTransform->maxY = childTransform->minY + desiredSize.y / window->GetContentScale();
            childTransform->rect = util::CalculateScreenSpaceRect(*childTransform, rectTransform);
            totalChildSize += childTransform->rect.size.y;
            LayoutWidgetChildren(world, system, child, *childTransform, layer, widgetComp);
            childTransform->needsLayout = false;

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
