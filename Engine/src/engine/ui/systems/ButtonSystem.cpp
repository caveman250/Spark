#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "ButtonSystem.h"

#include "engine/Application.h"
#include "engine/input/InputUtil.h"
#include "engine/ui/button/ButtonSubscription.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ButtonSystem)

    void ButtonSystem::OnUpdate(const std::vector<ecs::Id>& entities,
        const components::RectTransformComponent* rectTransforms,
        components::ButtonComponent* buttons,
        components::ImageComponent* images,
        input::InputComponent* inputComp)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            const auto& transform = rectTransforms[i];
            auto& button = buttons[i];
            auto& image = images[i];

            button.hovered = false;
            bool hovered = transform.rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY));

            if (hovered)
            {
                for (const auto& mouseEvent : inputComp->mouseEvents)
                {
                    if (mouseEvent.button == input::MouseButton::Left)
                    {
                        if (mouseEvent.state == input::KeyState::Down)
                        {
                            button.pressed = true;
                            input::InputUtil::ConsumeMouseEvent(mouseEvent, inputComp);
                            break;
                        }
                        else if (mouseEvent.state == input::KeyState::Up)
                        {
                            button.pressed = false;
                            input::InputUtil::ConsumeMouseEvent(mouseEvent, inputComp);
                            break;
                        }
                    }
                }
            }

            if (!button.pressed)
            {
                button.hovered = hovered;
            }

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

            button.lastHovered = button.hovered;
            button.lastPressed = button.pressed;

            for (auto& sub : button.subscriptions)
            {
                sub->UpdateFromButton(button);
            }
        }
    }
}
