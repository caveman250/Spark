#include "ContextMenuSystem.h"

#include "engine/input/InputComponent.h"
#include "engine/ui/components/ContextMenuComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

namespace se::ui::systems
{
    ecs::SystemDeclaration ContextMenuSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<const components::MouseInputComponent>()
            .WithComponent<const components::ContextMenuComponent>()
            .WithSingletonComponent<const input::InputComponent>();
    }

    void ContextMenuSystem::OnUpdate(const ecs::SystemUpdateData& systemUpdateData)
    {
        const auto* inputComp = systemUpdateData.GetSingletonComponent<const input::InputComponent>();
        if (inputComp->mouseButtonStates[static_cast<int>(input::MouseButton::Left)] == input::KeyState::Down)
        {
            auto world = Application::Get()->GetWorld();

            const auto& entities = systemUpdateData.GetEntities();
            const auto* mouseInputComps = systemUpdateData.GetComponentArray<const components::MouseInputComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                const auto& mouseInputComp = mouseInputComps[i];

                if (!mouseInputComp.hovered)
                {
                    world->DestroyEntity(entities[i]);
                }
            }
        }
    }
}
