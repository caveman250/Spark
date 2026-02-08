#include "TreeNodeSystem.h"
#include "UIMouseInputSystem.h"
#include "engine/ui/util/ContextMenuUtil.h"

namespace se::ui::systems
{
    ecs::SystemDeclaration TreeNodeSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Tree Node System")
                    .WithComponent<components::TreeNodeComponent>()
                    .WithComponent<components::MouseInputComponent>()
                    .WithSingletonComponent<const input::InputComponent>()
                    .WithDependency<UIMouseInputSystem>();
    }

    void TreeNodeSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto* inputComp = updateData.GetSingletonComponent<const input::InputComponent>();
        auto* treeNodes = updateData.GetComponentArray<components::TreeNodeComponent>();
        auto* mouseEventComps = updateData.GetComponentArray<components::MouseInputComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
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
                else if (mouseEvent.button == input::MouseButton::Right)
                {
                    if (mouseEvent.state == input::KeyState::Up)
                    {
                        util::ContextMenuParams params = {
                            .fontSize = 14,
                            .mousePos = { inputComp->mouseX, inputComp->mouseY },
                            .scene = Application::Get()->GetEditorRuntime()->GetEditorScene(),
                            .options = treeNode.contextOptions,
                        };

                        util::CreateContextMenu(params);
                    }
                }
            }
        }
    }
}
