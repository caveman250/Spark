#include "WindowObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

namespace se::ui::observers
{
    void WindowObserver::OnAdded(ecs::Id entity, components::WindowComponent *component)
    {
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        //background
        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto image = world->AddComponent<components::ImageComponent>(entity);

            auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/flat_color.sass");
            image->material = render::Material::CreateMaterial({vert}, {frag});
            image->material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.3f, 0.3f, 0.3f));
        }

        if (!world->HasComponent<ui::components::ReceivesMouseEventsComponent>(entity))
        {
            auto inputComp = world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
            inputComp->buttonMask = static_cast<input::MouseButton::Type>(0x0);
        }

        auto titleBarEntity = world->CreateEntity();
        auto titleBar = world->AddComponent<components::TitleBarComponent>(titleBarEntity);
        titleBar->onMove.Subscribe<components::RectTransformComponent>(entity, [](float dX, float dY, components::RectTransformComponent* transform)
        {
            transform->minX += dX;
            transform->maxX += dX;
            transform->minY += dY;
            transform->maxY += dY;
        });
        auto titleBarTransform = world->AddComponent<components::RectTransformComponent>(titleBarEntity);
        titleBarTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        titleBarTransform->minX = 0;
        titleBarTransform->maxX = 0;
        titleBarTransform->minY = 0;
        titleBarTransform->maxY = 40.f;
        world->AddChild(entity, titleBarEntity);

        auto buttonEntity = world->CreateEntity();
        auto buttonComp = world->AddComponent<components::ButtonComponent>(buttonEntity);
        buttonComp->image = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button.sass");
        buttonComp->pressedImage = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button_pressed.sass");
        buttonComp->hoveredImage = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_button_hovered.sass");
        buttonComp->onReleased.Subscribe<components::WindowComponent>(entity, [entity, world](components::WindowComponent*)
        {
            world->DestroyEntity(entity);
        });
        auto buttonTransform = world->AddComponent<components::RectTransformComponent>(buttonEntity);
        buttonTransform->anchors = { 1.f, 1.f, 0.f, 1.f };
        buttonTransform->minX = -35;
        buttonTransform->maxX = 5;
        buttonTransform->minY = 5;
        buttonTransform->maxY = 5;

        world->AddChild(titleBarEntity, buttonEntity);
    }

    void WindowObserver::OnRemoved(ecs::Id, components::WindowComponent*)
    {

    }
}
