#include "ButtonSystem.h"

#include "spark.h"
#include "UIMouseInputSystem.h"
#include "engine/Application.h"
#include <easy/profiler.h>
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
                    .WithSingletonComponent<const input::InputComponent>()
                    .WithDependency<UIMouseInputSystem>();
    }

    void ButtonSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        EASY_BLOCK("ButtonSystem::OnUpdate");

        auto* buttons = updateData.GetComponentArray<components::ButtonComponent>();
        auto* images = updateData.GetComponentArray<components::ImageComponent>();
        const auto* mouseEventComps = updateData.GetComponentArray<const components::MouseInputComponent>();
        const auto* inputComp = updateData.GetSingletonComponent<const input::InputComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
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

            if (button.pressed && inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] != input::KeyState::Down)
            {
                button.pressed = false;
            }

            button.hovered = !button.pressed && mouseEventComp.hovered;

            if (!image.materialInstance)
            {
                static auto material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/alpha_texture.sass") });
                auto rs = render::RenderState();
                rs.srcBlend = render::BlendMode::SrcAlpha;
                rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
                material->SetRenderState(rs);

                image.materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
                image.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &button.image);
            }

            asset::AssetReference<asset::Texture>* desiredTexture = nullptr;
            if (button.hovered)
            {
                desiredTexture = &button.hoveredImage;
            }
            else if (button.pressed)
            {
                desiredTexture = &button.pressedImage;
            }
            else if (!button.pressed && !button.hovered)
            {
                desiredTexture = &button.image;
            }

            if (SPARK_VERIFY(desiredTexture))
            {
                auto* currentTexture = image.materialInstance->GetUniform<asset::AssetReference<asset::Texture>>("Texture");
                if (currentTexture->GetAssetPath() != desiredTexture->GetAssetPath())
                {
                    image.materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, desiredTexture);
                }
            }

            if (button.pressed && !button.lastPressed)
            {
                button.pressedPosition = { inputComp->mouseX, inputComp->mouseY };
                button.onPressed.Broadcast();
            }

            if (button.pressed && !button.isDragging)
            {
                math::IntVec2 mousePos = { inputComp->mouseX, inputComp->mouseY };

                if (math::MagnitudeSquared(mousePos - button.pressedPosition) > 20 * 20)
                {
                    button.isDragging = true;
                    button.onDragged.Broadcast();
                }
            }

            if (!button.pressed && button.lastPressed && !button.isDragging)
            {
                button.onReleased.Broadcast();
            }

            button.lastHovered = button.hovered;
            button.lastPressed = button.pressed;
            button.isDragging &= button.pressed;
        }
    }
}
