#include "spark.h"

#include "TitleBarSystem.h"
#include <easy/profiler.h>
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/systems/UIMouseInputSystem.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration TitleBarSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("TitleBarSystem")
                    .WithComponent<components::TitleBarComponent>()
                    .WithComponent<components::MouseInputComponent>()
                    .WithComponent<const components::RectTransformComponent>()
                    .WithSingletonComponent<input::InputComponent>()
                    .WithDependency<UIMouseInputSystem>();
    }

    void TitleBarSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("TitleBarSystem::OnUpdate");

        auto* titleBars = updateData.GetComponentArray<components::TitleBarComponent>();
        const auto* transforms = updateData.GetComponentArray<const components::RectTransformComponent>();
        auto* mouseEventComps = updateData.GetComponentArray<components::MouseInputComponent>();
        auto* inputComp = updateData.GetSingletonComponent<input::InputComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
        {
            auto& titleBar = titleBars[i];
            const auto& rectTransform = transforms[i];
            auto& mouseEventComp = mouseEventComps[i];

            mouseEventComp.enabled = std::abs(inputComp->mouseY - rectTransform.rect.topLeft.y) >= 5;

            for (const auto& event : mouseEventComp.mouseEvents)
            {
                if (event.button == input::MouseButton::Left)
                {
                    if (event.state == input::KeyState::Down)
                    {
                        titleBar.pressed = true;
                    }
                    else if (event.state == input::KeyState::Up)
                    {
                        titleBar.pressed = false;
                    }
                }
            }

            if (titleBar.pressed && (inputComp->mouseDeltaX || inputComp->mouseDeltaY))
            {
                titleBar.onMove.Broadcast(static_cast<float>(inputComp->mouseDeltaX), static_cast<float>(inputComp->mouseDeltaY));
            }

            titleBar.lastPressed = titleBar.pressed;
        }
    }
}
