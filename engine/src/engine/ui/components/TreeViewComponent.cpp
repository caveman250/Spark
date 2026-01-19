#include "TreeViewComponent.h"

#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    int MeasureAndArrange(ecs::World* world,
        const ecs::Id& entity,
        ecs::System* system,
        components::RectTransformComponent& parentRect,
        bool collapsed,
        int startY)
    {
        auto dec = ecs::HeirachyQueryDeclaration()
            .WithComponent<components::RectTransformComponent>()
            .WithComponent<components::WidgetComponent>();
        int currentY = startY;
        util::ForEachWidgetChild(entity, system, dec,
        [world, collapsed, &currentY, system, parentRect](const ecs::SystemUpdateData& updateData, auto&& widgetComp)
        {
            auto window = Application::Get()->GetWindow();
            const auto& child = updateData.GetEntity();
            auto* widget = updateData.GetComponentArray<components::WidgetComponent>();
            auto* rectTransform = updateData.GetComponentArray<components::RectTransformComponent>();

            using Type = std::decay_t<decltype(widgetComp)>;
            if constexpr (std::is_same_v<Type, components::TreeNodeComponent*>)
            {
                widget->dirty = (widget->visibility == Visibility::Hidden) != collapsed;
                widget->visibility = collapsed ? Visibility::Hidden : Visibility::Visible; // TODO should be collapsed instead of Hidden.

                rectTransform->rect.topLeft.y = static_cast<int>(currentY * window->GetContentScale()) + parentRect.rect.topLeft.y;
                rectTransform->rect.topLeft.x = static_cast<int>(6 * window->GetContentScale()) + parentRect.rect.topLeft.x;
                rectTransform->rect.size.y = static_cast<int>(18 * window->GetContentScale());
                rectTransform->rect.size.x = parentRect.rect.size.x;
                rectTransform->layer = parentRect.layer;
                rectTransform->needsLayout = false;

                currentY += MeasureAndArrange(world, child, system, *rectTransform, collapsed || widgetComp->collapsed, 18);
            }
            else
            {
                rectTransform->rect = util::CalculateScreenSpaceRect(*rectTransform, parentRect);
                rectTransform->layer = parentRect.layer;
                Layout::LayoutWidgetChildren(world, system, child, *rectTransform, parentRect.layer, widgetComp);
            }

            return false;
        });

        return collapsed ? 18 : currentY;
    }

    template<>
    void Layout::LayoutWidgetChildren<components::TreeViewComponent>(ecs::World* world,
                                                             ecs::System* system,
                                                             const ecs::Id& entity,
                                                             components::RectTransformComponent& rectTransform,
                                                             [[maybe_unused]] int layer,
                                                             components::TreeViewComponent* treeView)
    {
        int maxY = MeasureAndArrange(world, entity, system, rectTransform, false, 0);
        rectTransform.maxY = static_cast<float>(maxY);
        rectTransform.needsLayout = false;
        treeView->dirty = false;
    }
}
