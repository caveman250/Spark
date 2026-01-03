#include "WindowUtil.h"

#include <engine/ui/components/TextComponent.h>

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/WindowComponent.h"

namespace se::ui::util
{
    ecs::Id CreateWindow(RectTransformComponent** transform,
                         WindowComponent** window,
                         TitleBarComponent** titleBar,
                         ecs::Id& childArea,
                         const std::string& title,
                         std::function<void()> onClose,
                         const bool editorOnly)
    {
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        ecs::Id entity = world->CreateEntity("Window", editorOnly);
        *transform = world->AddComponent<RectTransformComponent>(entity);
        *window = world->AddComponent<WindowComponent>(entity);

        //background
        if (!world->HasComponent<ImageComponent>(entity))
        {
            auto image = world->AddComponent<ImageComponent>(entity);

            static std::shared_ptr<render::Material> material = nullptr;
            if (!material)
            {
                material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
                material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.3f, 0.3f, 0.3f));
            }

            image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        }

        if (!world->HasComponent<MouseInputComponent>(entity))
        {
            auto inputComp = world->AddComponent<MouseInputComponent>(entity);
            inputComp->buttonMask = std::to_underlying(input::MouseButton::Left);
        }

        if (!world->HasComponent<KeyInputComponent>(entity))
        {
            auto inputComp = world->AddComponent<KeyInputComponent>(entity);
            inputComp->keyMask = static_cast<input::Key>(0x0);
        }

        auto titleBarEntity = world->CreateEntity("TitleBar", editorOnly);
        *titleBar = world->AddComponent<TitleBarComponent>(titleBarEntity);
        std::function moveCb = [entity](float dX, float dY)
        {
            auto window = Application::Get()->GetWorld()->GetComponent<WindowComponent>(entity);
            window->pendingDeltaX += dX;
            window->pendingDeltaY += dY;
        };
        (*titleBar)->onMove.Subscribe(std::move(moveCb));
        auto titleBarTransform = world->AddComponent<RectTransformComponent>(titleBarEntity);
        titleBarTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        titleBarTransform->minX = 0;
        titleBarTransform->maxX = 0;
        titleBarTransform->minY = 0;
        titleBarTransform->maxY = 30;
        world->AddChild(entity, titleBarEntity);

        ecs::Id titleBarTextEntity = world->CreateEntity("TitleBarText", true);
        auto titleBarText = world->AddComponent<TextComponent>(titleBarTextEntity);
        titleBarText->font = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        titleBarText->fontSize = 18;
        titleBarText->text = title;
        auto titleBarTextTransform = world->AddComponent<RectTransformComponent>(titleBarTextEntity);
        titleBarTextTransform->minX = 4;
        titleBarTextTransform->minY = 6;
        titleBarTextTransform->anchors = { 0.f, 1.f, 0.f, 1.f };
        world->AddComponent<WidgetComponent>(titleBarTextEntity);
        world->AddChild(titleBarEntity, titleBarTextEntity);

        auto buttonEntity = world->CreateEntity("Close Button", editorOnly);
        auto buttonComp = world->AddComponent<ButtonComponent>(buttonEntity);
        buttonComp->image = "/engine_assets/textures/close_button_idle.sass";
        buttonComp->pressedImage = "/engine_assets/textures/close_button_pressed.sass";
        buttonComp->hoveredImage = "/engine_assets/textures/close_button_hovered.sass";
        std::function<void()> buttonCb = [entity, world, onClose]()
        {
            onClose();
            world->DestroyEntity(entity);
        };
        buttonComp->onReleased.Subscribe(std::move(buttonCb));
        auto buttonTransform = world->AddComponent<RectTransformComponent>(buttonEntity);
        buttonTransform->anchors = { 1.f, 1.f, 0.f, 1.f };
        buttonTransform->minX = 25;
        buttonTransform->maxX = 5;
        buttonTransform->minY = 5;
        buttonTransform->maxY = 5;

        world->AddChild(titleBarEntity, buttonEntity);

        childArea = world->CreateEntity("Content", editorOnly);
        auto childAreaTransform = world->AddComponent<RectTransformComponent>(childArea);
        childAreaTransform->anchors = { 0.f, 1.f, 0.f, 1.f };
        childAreaTransform->minX = 0;
        childAreaTransform->maxX = 0;
        childAreaTransform->minY = 30;
        childAreaTransform->maxY = 0;
        world->AddChild(entity, childArea);

        return entity;
    }
}
