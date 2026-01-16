#include "TreeViewSystem.h"

#include "RectTransformSystem.h"
#include "TreeNodeSystem.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::systems
{
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
        // const auto& entities = updateData.GetEntities();
        // auto* treeViews = updateData.GetComponentArray<components::TreeViewComponent>();
        // auto* rectTransforms = updateData.GetComponentArray<components::RectTransformComponent>();
        //
    }
}
