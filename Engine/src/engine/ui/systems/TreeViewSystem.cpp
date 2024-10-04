#include "TreeViewSystem.h"

#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(TreeViewSystem)

    constexpr int s_Padding = 2;

    void TreeViewSystem::OnUpdate(const std::vector<ecs::Id>& entities,
                                  components::TreeViewComponent* treeViews)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& treeView = treeViews[i];
            if (treeView.dirty)
            {
                MeasureAndArrange(entities[i], false, 0);
                treeView.dirty = false;
            }
        }
    }

    int TreeViewSystem::MeasureAndArrange(ecs::Id entity, bool collapsed, int y)
    {
        int maxY = y;
        int currentY = 18;
        auto func = [this, collapsed, &currentY, &maxY](const std::vector<ecs::Id>& children,
                    const components::TreeNodeComponent* treeNodes,
                    components::RectTransformComponent* rectTransforms,
                    components::WidgetComponent* widgets)
        {
            for (size_t j = 0; j < children.size(); ++j)
            {
                auto& widget = widgets[j];
                widget.renderingEnabled = !collapsed;

                auto& childTransform = rectTransforms[j];
                childTransform.minY = currentY;
                childTransform.minX = 6;
                childTransform.maxY = currentY + 18;
                childTransform.anchors.right = 1.f;
                childTransform.needsLayout = true;

                const auto& treeNode = treeNodes[j];
                currentY += MeasureAndArrange(children[j], collapsed || treeNode.collapsed, currentY);
                maxY = currentY;
            }
        };

        RunChildQuery<const components::TreeNodeComponent, components::RectTransformComponent, components::WidgetComponent>(entity, func);

        return collapsed ? 18 : maxY;
    }
}
