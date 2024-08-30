#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "ButtonSystem.h"

#include "engine/Application.h"
#include "engine/input/InputUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ButtonSystem)

    void ButtonSystem::OnUpdate(const std::vector<ecs::Id>& entities,
        components::ButtonComponent* buttons,
        components::ImageComponent* images,
        const components::ReceivesMouseEventsComponent* mouseEventComps)
    {
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
                image.material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &button.hoveredImage);
            }
            else if (button.pressed && !button.lastPressed)
            {
                image.material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &button.pressedImage);
            }
            else if (!button.pressed && !button.hovered && (button.lastPressed || button.lastHovered))
            {
                image.material->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &button.image);
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
