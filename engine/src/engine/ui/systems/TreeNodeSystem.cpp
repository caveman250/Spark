#include "TreeNodeSystem.h"

namespace se::ui::systems
{
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
