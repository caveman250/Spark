#include "TreeViewSystem.h"

#include "RectTransformSystem.h"
#include "TreeNodeSystem.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::systems
{
    constexpr int s_Padding = 2;

    ecs::SystemDeclaration TreeViewSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("TreeViewSystem")
                    .WithComponent<components::TreeViewComponent>()
                    .WithComponent<components::RectTransformComponent>()
                    .WithDependency<RectTransformSystem>()
                    .WithDependency<TreeNodeSystem>()
                    .WithHeirachyQuery<const components::TreeNodeComponent>()
                    .WithHeirachyQuery<components::RectTransformComponent>()
                    .WithHeirachyQuery<components::WidgetComponent>();
    }

    void TreeViewSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto& entities = updateData.GetEntities();
        auto* treeViews = updateData.GetComponentArray<components::TreeViewComponent>();
        auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& treeView = treeViews[i];
            auto& rectTransform = rectTransforms[i];
            if (treeView.dirty)
            {
                int maxY = MeasureAndArrange(entities[i], false, 0);
                rectTransform.maxY = static_cast<float>(maxY);
                rectTransform.needsLayout = true;
                treeView.dirty = false;
            }
        }
    }

    int TreeViewSystem::MeasureAndArrange(const ecs::Id& entity, bool collapsed, int startY)
    {
        int currentY = startY;
        auto func = [this, collapsed, &currentY](const ecs::SystemUpdateData& updateData)
        {
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
                childTransform.minY = static_cast<float>(currentY);
                childTransform.minX = 6;
                childTransform.maxY = static_cast<float>(currentY + 18);
                childTransform.anchors.right = 1.f;
                childTransform.needsLayout = true;

                const auto& treeNode = treeNodes[j];
                currentY += MeasureAndArrange(children[j], collapsed || treeNode.collapsed, 18);
            }

            return false;
        };

        auto declaration = ecs::HeirachyQueryDeclaration()
            .WithComponent<const components::TreeNodeComponent>()
            .WithComponent<components::RectTransformComponent>()
            .WithComponent<components::WidgetComponent>();
        RunChildQuery(entity, declaration, func);

        return collapsed ? 18 : currentY;
    }
}
