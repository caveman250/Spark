#include "WindowUtil.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/WindowComponent.h"

namespace se::ui::util
{
    ecs::Id CreateWindow(components::RectTransformComponent** transform,
                         components::WindowComponent** window,
                         components::TitleBarComponent** titleBar,
                         ecs::Id& childArea)
    {
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        ecs::Id entity = world->CreateEntity("Window");
        *transform = world->AddComponent<components::RectTransformComponent>(entity);
        *window = world->AddComponent<components::WindowComponent>(entity);

        //background
        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto image = world->AddComponent<components::ImageComponent>(entity);

            auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/flat_color.sass");
            image->material = render::Material::CreateMaterial({vert}, {frag});
            image->material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.3f, 0.3f, 0.3f));
        }

        if (!world->HasComponent<components::ReceivesMouseEventsComponent>(entity))
        {
            auto inputComp = world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
            inputComp->buttonMask = static_cast<input::MouseButton::Type>(0x0);
        }

        auto titleBarEntity = world->CreateEntity("TitleBar");
        *titleBar = world->AddComponent<components::TitleBarComponent>(titleBarEntity);
        std::function<void(float, float, components::RectTransformComponent*)> moveCb = [](float dX, float dY, components::RectTransformComponent* transform)
        {
            transform->minX += static_cast<int>(dX);
            transform->maxX += static_cast<int>(dX);
            transform->minY += static_cast<int>(dY);
            transform->maxY += static_cast<int>(dY);
        };
        (*titleBar)->onMove.Subscribe<components::RectTransformComponent>(entity, std::move(moveCb));
        auto titleBarTransform = world->AddComponent<components::RectTransformComponent>(titleBarEntity);
        titleBarTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        titleBarTransform->minX = 0;
        titleBarTransform->maxX = 0;
        titleBarTransform->minY = 0;
        titleBarTransform->maxY = 40;
        world->AddChild(entity, titleBarEntity);

        auto buttonEntity = world->CreateEntity("Close Button");
        auto buttonComp = world->AddComponent<components::ButtonComponent>(buttonEntity);
        buttonComp->image = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button.sass");
        buttonComp->pressedImage = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button_pressed.sass");
        buttonComp->hoveredImage = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button_hovered.sass");
        std::function<void(components::WindowComponent*)> buttonCb = [entity, world](components::WindowComponent*)
        {
            world->DestroyEntity(entity);
        };
        buttonComp->onReleased.Subscribe<components::WindowComponent>(entity, std::move(buttonCb));
        auto buttonTransform = world->AddComponent<components::RectTransformComponent>(buttonEntity);
        buttonTransform->anchors = { 1.f, 1.f, 0.f, 1.f };
        buttonTransform->minX = -35;
        buttonTransform->maxX = 5;
        buttonTransform->minY = 5;
        buttonTransform->maxY = 5;

        world->AddChild(titleBarEntity, buttonEntity);

        childArea = world->CreateEntity("Content");
        auto childAreaTransform = world->AddComponent<components::RectTransformComponent>(childArea);
        childAreaTransform->anchors = { 0.f, 1.f, 0.f, 1.f };
        childAreaTransform->minX = 0;
        childAreaTransform->maxX = 0;
        childAreaTransform->minY = 40;
        childAreaTransform->maxY = 0;
        world->AddChild(entity, childArea);

        return entity;
    }
}
