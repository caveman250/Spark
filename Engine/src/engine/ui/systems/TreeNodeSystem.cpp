#include "TreeNodeSystem.h"
#include <stacktrace>
namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(TreeNodeSystem)

    void TreeNodeSystem::OnUpdate(const std::vector<ecs::Id>& entities,
                                  components::TreeNodeComponent* treeNodes,
                                  components::ReceivesMouseEventsComponent* mouseEventComps)
    {
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
