#include "ContextMenuUtil.h"

#include "engine/Application.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/asset/font/Font.h"
#include "engine/render/Material.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ContextMenuComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

namespace se::ui::util
{
    void ContextMenuParams::AddOption(const std::string& name,
        const std::function<void()>& cb)
    {
        options.push_back(std::make_pair(name, cb));
    }

    void CreateContextMenu(ContextMenuParams& params)
    {
        ecs::Id contextMenu = {};

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto window = app->GetWindow();
        auto assetManager = asset::AssetManager::Get();
        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        contextMenu = world->CreateEntity(params.scene, "Context Menu");
        auto verticalBoxTransform = world->AddComponent<RectTransformComponent>(contextMenu);
        verticalBoxTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        verticalBoxTransform->layer = -1;
        verticalBoxTransform->minY = params.mousePos.y / window->GetContentScale();
        verticalBoxTransform->minX = params.mousePos.x / window->GetContentScale();
        verticalBoxTransform->maxX = verticalBoxTransform->minX + params.minWidth;
        world->AddComponent<WidgetComponent>(contextMenu);
        world->AddComponent<MouseInputComponent>(contextMenu);
        world->AddComponent<ContextMenuComponent>(contextMenu);
        auto verticalBox = world->AddComponent<VerticalBoxComponent>(contextMenu);
        verticalBox->paddingTop = 2;
        verticalBox->paddingLeft = 2;
        verticalBox->paddingRight = 2;
        auto image = world->AddComponent<ImageComponent>(contextMenu);
        auto bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_context_menu.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        for (size_t i = 0; i < params.options.size(); ++i)
        {
            const auto& option = params.options[i];

            auto textEntity = world->CreateEntity(params.scene, "Label");
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = params.fontSize;
            text->text = option.first;
            world->AddComponent<RectTransformComponent>(textEntity);
            world->AddComponent<WidgetComponent>(textEntity);
            world->AddChild(contextMenu, textEntity);

            auto textButtonEntity = world->CreateEntity(params.scene, "Button");
            auto buttonRect = world->AddComponent<RectTransformComponent>(textButtonEntity);
            buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
            auto textButton = world->AddComponent<ButtonComponent>(textButtonEntity);
            textButton->onReleased.Subscribe([world, option, contextMenu](input::MouseButton)
            {
                option.second();
                world->DestroyEntity(contextMenu);
            });
            auto optionWidget = world->AddComponent<WidgetComponent>(textButtonEntity);
            optionWidget->visibility = Visibility::Hidden;
            world->AddChild(textEntity, textButtonEntity);
        }
    }
}
