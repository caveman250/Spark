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

        ret.comboBox->collapsedEntity = world->CreateEntity("ComboBox Collapsed", params.editorOnly);
        auto bgTransform = world->AddComponent<RectTransformComponent>(ret.comboBox->collapsedEntity);
        bgTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddComponent<WidgetComponent>(ret.comboBox->collapsedEntity);
        auto image = world->AddComponent<ImageComponent>(ret.comboBox->collapsedEntity);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto flat_color = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = render::Material::CreateMaterial({vert}, {flat_color});
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        auto innerImageEntity = world->CreateEntity("Border", params.editorOnly);
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = render::Material::CreateMaterial({vert}, {flat_color});
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(ret.comboBox->collapsedEntity, innerImageEntity);

        collapsedText = world->CreateEntity("Label", params.editorOnly);
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

        ecs::Id titleIndicator = world->CreateEntity("Collapsing Header Title Indicator", params.editorOnly);
        auto indicatorRect = world->AddComponent<RectTransformComponent>(titleIndicator);
        indicatorRect->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        indicatorRect->minX = 12;
        indicatorRect->maxX = 4;
        indicatorRect->minY = 4;
        indicatorRect->maxY = 12;
        world->AddComponent<WidgetComponent>(titleIndicator);
        auto indicatorImage = world->AddComponent<ImageComponent>(titleIndicator);
        auto alpha_texture = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/alpha_texture.sass");
        static std::shared_ptr<render::Material> indicatorMaterial = nullptr;
        if (!indicatorMaterial)
        {
            indicatorMaterial = render::Material::CreateMaterial({ vert }, { alpha_texture });
            auto rs = render::RenderState();
            rs.srcBlend = render::BlendMode::SrcAlpha;
            rs.dstBlend = render::BlendMode::OneMinusSrcAlpha;
            indicatorMaterial->SetRenderState(rs);
        }
        static auto expanded_indicator_texture = assetManager->Get()->GetAsset<asset::Texture>("/engine_assets/textures/tree_node_indicator_expanded.sass");
        indicatorImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(indicatorMaterial);
        indicatorImage->materialInstance->SetUniform("Texture",
                                                     asset::shader::ast::AstType::Sampler2D,
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

        ret.comboBox->expandedEntity = world->CreateEntity("ComboBox Expanded", params.editorOnly);
        auto bgTransform = world->AddComponent<RectTransformComponent>(ret.comboBox->expandedEntity);
        bgTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(ret.comboBox->expandedEntity);
        auto image = world->AddComponent<ImageComponent>(ret.comboBox->expandedEntity);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto flat_color = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = render::Material::CreateMaterial({vert}, {flat_color});
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);

        auto innerImageEntity = world->CreateEntity("Border", params.editorOnly);
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = render::Material::CreateMaterial({vert}, {flat_color});
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(ret.comboBox->expandedEntity, innerImageEntity);

        auto verticalBoxEntity = world->CreateEntity("Vertical Box", params.editorOnly);
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

            auto textEntity = world->CreateEntity("Label", params.editorOnly);
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = params.fontSize;
            text->text = option;
            world->AddComponent<RectTransformComponent>(textEntity);
            world->AddComponent<WidgetComponent>(textEntity);
            world->AddChild(verticalBoxEntity, textEntity);

            auto textButtonEntity = world->CreateEntity("Button", params.editorOnly);
            auto buttonRect = world->AddComponent<RectTransformComponent>(textButtonEntity);
            buttonRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
            auto textButton = world->AddComponent<ButtonComponent>(textButtonEntity);
            textButton->onReleased.Subscribe([params, ret, i, world, collapsedText, option]()
            {
                params.onItemSelected(i);
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

        ret.id = world->CreateEntity("ComboBox", params.editorOnly);
        ret.rectTransform = world->AddComponent<RectTransformComponent>(ret.id);
        ret.rectTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        ret.rectTransform->minY = 0;
        ret.rectTransform->maxY = static_cast<int>(arial->GetLineHeight(params.fontSize) + padding + borderSize * 2 + 0.5f);
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
