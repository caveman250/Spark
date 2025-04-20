#include "TreeViewSystem.h"

#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(TreeViewSystem)

    constexpr int s_Padding = 2;

    void TreeViewSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto& entities = updateData.GetEntities();
        auto* treeViews = updateData.GetComponentArray<ui::components::TreeViewComponent>();
        auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& treeView = treeViews[i];
            auto& rectTransform = rectTransforms[i];
            if (treeView.dirty)
            {
                int maxY = MeasureAndArrange(entities[i], false, 0);
                rectTransform.maxY = maxY;
                rectTransform.needsLayout = true;
                treeView.dirty = false;
            }
        }
    }

    int TreeViewSystem::MeasureAndArrange(ecs::Id entity, bool collapsed, int startY)
    {
        int currentY = startY;
        auto func = [this, collapsed, &currentY](const ecs::SystemUpdateData& updateData)
        {
            const auto& children = updateData.GetEntities();
            auto* widgets = updateData.GetComponentArray<ui::components::WidgetComponent>();
            auto* rectTransforms = updateData.GetComponentArray<ui::components::RectTransformComponent>();
            const auto* treeNodes = updateData.GetComponentArray<const ui::components::TreeNodeComponent>();

            for (size_t j = 0; j < children.size(); ++j)
            {
                auto& widget = widgets[j];
                widget.dirty = widget.renderingEnabled == collapsed;
                widget.renderingEnabled = !collapsed;

                auto& childTransform = rectTransforms[j];
                childTransform.minY = currentY;
                childTransform.minX = 6;
                childTransform.maxY = currentY + 18;
                childTransform.anchors.right = 1.f;
                childTransform.needsLayout = true;

                const auto& treeNode = treeNodes[j];
                currentY += MeasureAndArrange(children[j], collapsed || treeNode.collapsed, 18);
            }

            return false;
        };

        auto declaration = ecs::ChildQueryDeclaration()
            .WithComponent<const components::TreeNodeComponent>()
            .WithComponent<components::RectTransformComponent>()
            .WithComponent<components::WidgetComponent>();
        RunChildQuery(entity, declaration, func);

        return collapsed ? 18 : currentY;
    }
}
