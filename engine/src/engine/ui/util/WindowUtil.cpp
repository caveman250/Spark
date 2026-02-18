#include "WindowUtil.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/WindowComponent.h"

namespace se::ui::util
{
    ecs::Id CreateWindow(RectTransformComponent** transform,
                         WindowComponent** window,
                         TitleBarComponent** titleBar,
                         ecs::Id& childArea,
                         ecs::Id& titleArea,
                         const std::string& title,
                         const ecs::Id& scene)
    {
        const auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        ecs::Id entity = world->CreateEntity(scene, "Window");
        *transform = world->AddComponent<RectTransformComponent>(entity);
        *window = world->AddComponent<WindowComponent>(entity);

        //background
        if (!world->HasComponent<ImageComponent>(entity))
        {
            const auto image = world->AddComponent<ImageComponent>(entity);

            const auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_window_bg.sass");
            image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        }

        if (!world->HasComponent<MouseInputComponent>(entity))
        {
            world->AddComponent<MouseInputComponent>(entity);
        }

        if (!world->HasComponent<KeyInputComponent>(entity))
        {
            const auto inputComp = world->AddComponent<KeyInputComponent>(entity);
            inputComp->keyMask = static_cast<input::Key>(0x0);
        }

        titleArea = world->CreateEntity(scene, "TitleBar");
        *titleBar = world->AddComponent<TitleBarComponent>(titleArea);
        std::function moveCb = [entity](const float dX, const float dY)
        {
            const auto windowComp = Application::Get()->GetWorld()->GetComponent<WindowComponent>(entity);
            windowComp->pendingDeltaX += dX;
            windowComp->pendingDeltaY += dY;
        };
        (*titleBar)->onMove.Subscribe(std::move(moveCb));
        const auto titleBarTransform = world->AddComponent<RectTransformComponent>(titleArea);
        titleBarTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        titleBarTransform->minX = 0;
        titleBarTransform->maxX = 0;
        titleBarTransform->minY = 0;
        titleBarTransform->maxY = 30;
        world->AddChild(entity, titleArea);

        const ecs::Id titleBarTextEntity = world->CreateEntity(scene, "TitleBarText");
        auto* titleBarText = world->AddComponent<TextComponent>(titleBarTextEntity);
        titleBarText->font = "/engine_assets/fonts/Arial.sass";
        titleBarText->fontSize = 18;
        titleBarText->text = title;
        auto* titleBarTextTransform = world->AddComponent<RectTransformComponent>(titleBarTextEntity);
        titleBarTextTransform->minX = 4;
        titleBarTextTransform->minY = 6;
        titleBarTextTransform->anchors = { 0.f, 1.f, 0.f, 1.f };
        world->AddComponent<WidgetComponent>(titleBarTextEntity);
        world->AddChild(titleArea, titleBarTextEntity);

        childArea = world->CreateEntity(scene, "Content");
        auto* childAreaTransform = world->AddComponent<RectTransformComponent>(childArea);
        childAreaTransform->anchors = { 0.f, 1.f, 0.f, 1.f };
        childAreaTransform->minX = 0;
        childAreaTransform->maxX = 0;
        childAreaTransform->minY = 30;
        childAreaTransform->maxY = 0;
        world->AddChild(entity, childArea);

        return entity;
    }
}
