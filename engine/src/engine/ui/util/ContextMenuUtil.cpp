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
#include "engine/ui/util/ScrollBoxUtil.h"

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
        auto contextMenuTransform = world->AddComponent<RectTransformComponent>(contextMenu);
        contextMenuTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        contextMenuTransform->layer = -1;
        contextMenuTransform->minY = params.mousePos.y / window->GetContentScale();
        contextMenuTransform->maxY = contextMenuTransform->minY + 1;
        contextMenuTransform->minX = params.mousePos.x / window->GetContentScale();
        contextMenuTransform->maxX = contextMenuTransform->minX + params.minWidth;
        world->AddComponent<WidgetComponent>(contextMenu);
        world->AddComponent<MouseInputComponent>(contextMenu);
        world->AddComponent<ContextMenuComponent>(contextMenu);
        auto image = world->AddComponent<ImageComponent>(contextMenu);
        auto bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_context_menu.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        auto verticalBox = world->CreateEntity(params.scene, "Vertical Box");
        auto verticalBoxComp = world->AddComponent<VerticalBoxComponent>(verticalBox);
        verticalBoxComp->paddingTop = 2;
        verticalBoxComp->paddingLeft = 2;
        verticalBoxComp->paddingRight = 2;
        auto verticalBoxTransform = world->AddComponent<RectTransformComponent>(verticalBox);
        verticalBoxTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        verticalBoxTransform->layer = -1;
        auto verticalBoxWidget = world->AddComponent<WidgetComponent>(verticalBox);
        world->AddComponent<MouseInputComponent>(verticalBox);

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
            world->AddChild(verticalBox, textEntity);

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

        Rect windowRect = Rect {
            .topLeft = math::IntVec2(0, 0),
            .size = math::IntVec2(window->GetWidth(), window->GetHeight())
        };
        contextMenuTransform->rect = CalculateScreenSpaceRect(*contextMenuTransform, windowRect);
        auto desiredHeight = DesiredSizeCalculator::GetDesiredSize(params.system,
            verticalBox,
            verticalBoxWidget,
            *contextMenuTransform,
            *verticalBoxTransform,
            verticalBoxComp).y;

        if (desiredHeight < 400 * window->GetContentScale())
        {
            world->AddChild(contextMenu, verticalBox);
            contextMenuTransform->maxY = contextMenuTransform->minY + desiredHeight / window->GetContentScale();
        }
        else
        {
            contextMenuTransform->maxY = contextMenuTransform->minY + 400;
            auto scrollBox = CreateScrollBox(params.scene);
            world->AddChild(contextMenu, scrollBox.scrollBoxEntity);
            world->AddChild(scrollBox.scrollViewEntity, verticalBox);
        }

        auto windowSize = windowRect.size / window->GetContentScale();

        if (contextMenuTransform->maxX > windowSize.x)
        {
            int delta = static_cast<int>(contextMenuTransform->maxX) - windowSize.x;
            contextMenuTransform->maxX -= delta;
            contextMenuTransform->minX -= delta;
        }
        if (contextMenuTransform->maxY > windowSize.y)
        {
            int delta = static_cast<int>(contextMenuTransform->maxY) - windowSize.y;
            contextMenuTransform->maxY -= delta;
            contextMenuTransform->minY -= delta;
        }
    }
}
