#pragma once

#include "PropertyEditor.h"
#include "spark.h"
#include "engine/Application.h"
#include "engine/asset/texture/Texture.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/mesh/Model.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ecs/SceneSaveData.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::editor::ui::properties
{
    template <typename T>
    class AssetReferenceEditor : public PropertyEditor
    {
        SPARK_CLASS_TEMPLATED()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        void Update() override;

    private:
        asset::AssetReference<T>* m_Value = nullptr;
        ecs::Id m_Label;
    };

    SPARK_INSTANTIATE_TEMPLATE(AssetReferenceEditor, asset::Texture);
    SPARK_INSTANTIATE_TEMPLATE(AssetReferenceEditor, asset::Shader);
    SPARK_INSTANTIATE_TEMPLATE(AssetReferenceEditor, asset::Model);
    SPARK_INSTANTIATE_TEMPLATE(AssetReferenceEditor, asset::Font);
    SPARK_INSTANTIATE_TEMPLATE(AssetReferenceEditor, ecs::SceneSaveData)
    SPARK_INSTANTIATE_TEMPLATE(AssetReferenceEditor, render::Material)

    template<typename T>
    void AssetReferenceEditor<T>::SetValue(void* value,
        const reflect::Type*)
    {
        m_Value = static_cast<asset::AssetReference<T>*>(value);
    }

    template<typename T>
    void AssetReferenceEditor<T>::ConstructUI(const std::string& name,
        bool constructTitle,
        const se::ui::Anchors& anchors,
        bool collapsed,
        bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        constexpr int iconSize = 32;
        constexpr int borderSize = 2;

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity(editor->GetEditorScene(), "Asset Reference Editor");
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.35f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = iconSize + borderSize * 4;
        world->AddComponent<WidgetComponent>(bg);
        auto image = world->AddComponent<ImageComponent>(bg);
        auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_lightbg.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(m_Content, bg);

        auto innerImageEntity = world->CreateEntity(editor->GetEditorScene(), "Border");
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        auto innerMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/editor_darkbg.sass");
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(bg, innerImageEntity);

        auto iconEntity = world->CreateEntity(editor->GetEditorScene(), "Icon");
        auto iconImage = world->AddComponent<ImageComponent>(iconEntity);
        auto iconMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
        iconImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(iconMaterial);
        if (m_Value->IsSet())
        {
            auto fileImage = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_file.sass");
            iconImage->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &fileImage);
        }
        else
        {
            auto fileImage = asset::AssetReference<asset::Texture>("/engine_assets/textures/no_file.sass");
            iconImage->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &fileImage);
        }
        auto iconTransform = world->AddComponent<RectTransformComponent>(iconEntity);
        iconTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        iconTransform->minX = iconTransform->minY = borderSize;
        iconTransform->maxX = iconTransform->maxY = borderSize + iconSize;
        world->AddChild(innerImageEntity, iconEntity);

        auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Title");
        world->AddComponent<WidgetComponent>(textEntity);
        auto text = world->AddComponent<TextComponent>(textEntity);
        text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        text->fontSize = 14;
        if (m_Value->IsSet())
        {
            std::string smallPath = m_Value->GetAssetPath();
            auto lastSlashIt = smallPath.find_last_of('/');
            auto extensionIt = smallPath.find_last_of('.');
            std::string assetName = smallPath.substr(lastSlashIt + 1, extensionIt - 1 - lastSlashIt);
            text->text = assetName;
        }
        else
        {
            text->text = "None";
        }
        auto textRect = world->AddComponent<RectTransformComponent>(textEntity);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        textRect->minX = iconSize + borderSize * 3;
        textRect->minY = borderSize + 8;
        textRect->maxY = borderSize;
        world->AddChild(innerImageEntity, textEntity);
    }

    template<typename T>
    void AssetReferenceEditor<T>::Update()
    {

    }
}

#include "se_editor_ui_properties_AssetReferenceEditor.generated.h"
