#include "ButtonSystem.h"

#include "spark.h"
#include "UIMouseInputSystem.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration ButtonSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Button System")
                    .WithComponent<components::ButtonComponent>()
                    .WithComponent<components::ImageComponent>()
                    .WithComponent<const ui::components::MouseInputComponent>()
                    .WithDependency<UIMouseInputSystem>();
    }

    void ButtonSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        PROFILE_SCOPE("ButtonSystem::OnUpdate")

        const auto& entities = updateData.GetEntities();
        auto* buttons = updateData.GetComponentArray<components::ButtonComponent>();
        auto* images = updateData.GetComponentArray<components::ImageComponent>();
        const auto* mouseEventComps = updateData.GetComponentArray<const components::MouseInputComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& button = buttons[i];
            auto& image = images[i];
            auto& mouseEventComp = mouseEventComps[i];

            for (const auto& mouseEvent : mouseEventComp.mouseEvents)
            {
                if (mouseEvent.button == input::MouseButton::Left)
                {
                    if (mouseEvent.state == input::KeyState::Down)
                    {
                        button.pressed = true;
                    }
                    else if (mouseEvent.state == input::KeyState::Up)
                    {
                        button.pressed = false;
                    }
                }
            }

            button.hovered = !button.pressed && mouseEventComp.hovered;

            if (button.hovered && !button.lastHovered)
            {
                image.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &button.hoveredImage);
            }
            else if (button.pressed && !button.lastPressed)
            {
                image.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &button.pressedImage);
            }
            else if (!button.pressed && !button.hovered && (button.lastPressed || button.lastHovered))
            {
                image.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &button.image);
            }

            if (button.pressed && !button.lastPressed)
            {
                button.onPressed.Broadcast();
            }
            else if (!button.pressed && button.lastPressed)
            {
                button.onReleased.Broadcast();
            }

            button.lastHovered = button.hovered;
            button.lastPressed = button.pressed;
        }
    }
}
