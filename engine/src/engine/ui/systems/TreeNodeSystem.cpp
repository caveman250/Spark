#include "TreeNodeSystem.h"
#include "UIMouseInputSystem.h"

namespace se::ui::systems
{
    ecs::SystemDeclaration TreeNodeSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Tree Node System")
                    .WithComponent<components::TreeNodeComponent>()
                    .WithComponent<components::MouseInputComponent>()
                    .WithDependency<UIMouseInputSystem>();
    }

    void TreeNodeSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto& entities = updateData.GetEntities();
        auto* treeNodes = updateData.GetComponentArray<components::TreeNodeComponent>();
        auto* mouseEventComps = updateData.GetComponentArray<components::MouseInputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& treeNode = treeNodes[i];
            auto& eventComp = mouseEventComps[i];

            for (const auto& mouseEvent : eventComp.mouseEvents)
            {
                if (mouseEvent.button == input::MouseButton::Left)
                {
                    if (mouseEvent.state == input::KeyState::Up)
                    {
                        treeNode.onSelected.Broadcast();
                    }
                }
            }
        }
    }
}
