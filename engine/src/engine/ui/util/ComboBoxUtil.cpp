#include "ComboBoxUtil.h"
#include "Widgets.generated.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::util
{
    constexpr int padding = 4;
    constexpr int textYOffset = padding / 2;
    constexpr int borderSize = 2;

    void CreateCollapsedEntity(ComboBoxParams& params, NewComboBox& ret, ecs::Id& collapsedText)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        ret.comboBox->collapsedEntity = world->CreateEntity(params.scene, "ComboBox Collapsed");
        auto bgTransform = world->AddComponent<RectTransformComponent>(ret.comboBox->collapsedEntity);
        bgTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddComponent<WidgetComponent>(ret.comboBox->collapsedEntity);
        auto image = world->AddComponent<ImageComponent>(ret.comboBox->collapsedEntity);
        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        auto innerImageEntity = world->CreateEntity(params.scene, "Border");
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(ret.comboBox->collapsedEntity, innerImageEntity);

        collapsedText = world->CreateEntity(params.scene, "Label");
        auto text = world->AddComponent<TextComponent>(collapsedText);
        text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        text->fontSize = params.fontSize;
        text->text = params.options[params.selectedIndex];
        auto labelRect = world->AddComponent<RectTransformComponent>(collapsedText);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        labelRect->minX = 2;
        labelRect->maxX = 2;
        labelRect->minY = textYOffset;
        world->AddChild(innerImageEntity, collapsedText);

        ecs::Id titleIndicator = world->CreateEntity(params.scene, "Collapsing Header Title Indicator");
        auto indicatorRect = world->AddComponent<RectTransformComponent>(titleIndicator);
        indicatorRect->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        indicatorRect->minX = 12;
        indicatorRect->maxX = 4;
        indicatorRect->minY = 4;
        indicatorRect->maxY = 12;
        world->AddComponent<WidgetComponent>(titleIndicator);
        auto indicatorImage = world->AddComponent<ImageComponent>(titleIndicator);
        static std::shared_ptr<render::Material> indicatorMaterial = nullptr;
        if (!indicatorMaterial)
        {
            indicatorMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/alpha_texture.sass") });
            auto rs = render::RenderState();
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            indicatorMaterial->SetRenderState(rs);
        }
        static asset::AssetReference<asset::Texture> expanded_indicator_texture = "/engine_assets/textures/tree_node_indicator_expanded.sass";
        indicatorImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(indicatorMaterial);
        indicatorImage->materialInstance->SetUniform("Texture",
                                                     asset::shader::ast::AstType::Sampler2DReference,
                                                     1,
                                                     &expanded_indicator_texture);
        world->AddChild(innerImageEntity, titleIndicator);

        world->AddChild(ret.id, ret.comboBox->collapsedEntity);
    }

    void CreateExpandedEntity(ComboBoxParams& params, NewComboBox& ret, ecs::Id collapsedText)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        ret.comboBox->expandedEntity = world->CreateEntity(params.scene, "ComboBox Expanded");
        auto bgTransform = world->AddComponent<RectTransformComponent>(ret.comboBox->expandedEntity);
        bgTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(ret.comboBox->expandedEntity);
        auto image = world->AddComponent<ImageComponent>(ret.comboBox->expandedEntity);
        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        auto innerImageEntity = world->CreateEntity(params.scene, "Border");
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(ret.comboBox->expandedEntity, innerImageEntity);

        auto verticalBoxEntity = world->CreateEntity(params.scene, "Vertical Box");
        auto verticalBoxTransform = world->AddComponent<RectTransformComponent>(verticalBoxEntity);
        verticalBoxTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        verticalBoxTransform->overridesChildSizes = true;
        world->AddComponent<WidgetComponent>(verticalBoxEntity);
        auto verticalBox = world->AddComponent<VerticalBoxComponent>(verticalBoxEntity);
        verticalBox->paddingTop = textYOffset;
        verticalBox->paddingLeft = 2;
        verticalBox->paddingRight = 2;
        world->AddChild(innerImageEntity, verticalBoxEntity);

        for (size_t i = 0; i < params.options.size(); ++i)
        {
            const auto& option = params.options[i];

            auto textEntity = world->CreateEntity(params.scene, "Label");
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = params.fontSize;
            text->text = option;
            world->AddComponent<RectTransformComponent>(textEntity);
            world->AddComponent<WidgetComponent>(textEntity);
            world->AddChild(verticalBoxEntity, textEntity);

            auto textButtonEntity = world->CreateEntity(params.scene, "Button");
            auto buttonRect = world->AddComponent<RectTransformComponent>(textButtonEntity);
            buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
            auto textButton = world->AddComponent<ButtonComponent>(textButtonEntity);
            textButton->onReleased.Subscribe([params, ret, i, world, collapsedText, option]()
            {
                params.onItemSelected(static_cast<int>(i));
                auto comboBox = world->GetComponent<ComboBoxComponent>(ret.id);
                comboBox->collapsed = !comboBox->collapsed;

                auto text = world->GetComponent<TextComponent>(collapsedText);
                text->text = option;
            });
            auto optionWidget = world->AddComponent<WidgetComponent>(textButtonEntity);
            optionWidget->visibility = Visibility::Hidden;
            world->AddChild(textEntity, textButtonEntity);
        }

        world->AddChild(ret.id, ret.comboBox->expandedEntity);
    }

    NewComboBox CreateComboBox(ComboBoxParams& params)
    {
        NewComboBox ret = {};

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto arial = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        ret.id = world->CreateEntity(params.scene, "ComboBox");
        ret.rectTransform = world->AddComponent<RectTransformComponent>(ret.id);
        ret.rectTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        ret.rectTransform->minY = 0;
        ret.rectTransform->maxY = arial->GetLineHeight(params.fontSize) + padding + borderSize * 2 + 0.5f;
        ret.rectTransform->overridesChildSizes = true;
        world->AddComponent<WidgetComponent>(ret.id);
        ret.comboBox = world->AddComponent<ComboBoxComponent>(ret.id);
        world->AddComponent<MouseInputComponent>(ret.id);

        ecs::Id collapsedText = {};
        CreateCollapsedEntity(params, ret, collapsedText);
        CreateExpandedEntity(params, ret, collapsedText);

        return ret;
    }
}
