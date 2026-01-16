#include "TreeViewComponent.h"

#include "engine/ui/util/WidgetUtil.h"

namespace se::ui
{
    int MeasureAndArrange(const ecs::Id& entity,
        ecs::System* system,
        components::RectTransformComponent& parentRect,
        bool collapsed,
        int startY)
    {
        int currentY = startY;
        auto func = [collapsed, &currentY, system, parentRect](const ecs::SystemUpdateData& updateData)
        {
            auto window = Application::Get()->GetWindow();
            const auto& children = updateData.GetEntities();
            auto* widgets = updateData.GetComponentArray<components::WidgetComponent>();
            auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
            const auto* treeNodes = updateData.GetComponentArray<const components::TreeNodeComponent>();

            for (size_t j = 0; j < children.size(); ++j)
            {
                auto& widget = widgets[j];
                widget.dirty = (widget.visibility == Visibility::Hidden) != collapsed;
                widget.visibility = collapsed ? Visibility::Hidden : Visibility::Visible; // TODO should be collapsed instead of Hidden.

                auto& childTransform = rectTransforms[j];
                childTransform.rect.topLeft.y = currentY * window->GetContentScale();
                childTransform.rect.topLeft.x = 6 * window->GetContentScale();
                childTransform.rect.size.y = childTransform.rect.topLeft.y + 18 * window->GetContentScale();
                childTransform.rect.size.x = parentRect.rect.size.x;
                childTransform.needsLayout = false;

                const auto& treeNode = treeNodes[j];
                currentY += MeasureAndArrange(children[j], system, childTransform, collapsed || treeNode.collapsed, 18);
            }

            return false;
        };

        auto declaration = ecs::HeirachyQueryDeclaration()
            .WithComponent<const components::TreeNodeComponent>()
            .WithComponent<components::RectTransformComponent>()
            .WithComponent<components::WidgetComponent>();
        system->RunChildQuery(entity, declaration, func);

        return collapsed ? 18 : currentY;
    }

    template<>
    void Layout::LayoutWidgetChildren<components::TreeViewComponent>(ecs::World*,
                                                             ecs::System* system,
                                                             const ecs::Id& entity,
                                                             components::RectTransformComponent& rectTransform,
                                                             components::TreeViewComponent* treeView)
    {
        if (treeView->dirty)
        {
            int maxY = MeasureAndArrange(entity, system, rectTransform, false, 0);
            rectTransform.maxY = static_cast<float>(maxY);
            rectTransform.needsLayout = false;
            treeView->dirty = false;
        };
    }
}
