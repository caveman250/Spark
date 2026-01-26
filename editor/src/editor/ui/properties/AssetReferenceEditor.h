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
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "../../singleton_components/DragDropStateComponent.h"

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
        ecs::Id m_Root;
        ecs::Id m_Label;
        bool m_IsHighlighted = false;

        inline static const math::Vec3 s_DefaultColor = math::Vec3(0.6f, 0.6f, 0.6f);
        inline static const math::Vec3 s_GreenColor = math::Vec3(0.f, 1.f, 0.f);
        inline static const math::Vec3 s_RedColor = math::Vec3(1.f, 0.f, 0.f);
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

    inline std::string GetAssetName(const std::string& path)
    {
        std::string smallPath = path;
        auto lastSlashIt = smallPath.find_last_of('/');
        auto extensionIt = smallPath.find_last_of('.');
        return smallPath.substr(lastSlashIt + 1, extensionIt - 1 - lastSlashIt);
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

        m_Root = world->CreateEntity(editor->GetEditorScene(), "Asset Reference Editor");
        auto bgTransform = world->AddComponent<RectTransformComponent>(m_Root);
        bgTransform->anchors = { .left = constructTitle ? 0.35f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = iconSize + borderSize * 4;
        world->AddComponent<WidgetComponent>(m_Root);
        auto image = world->AddComponent<ImageComponent>(m_Root);
        auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_runtime_flat_color.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        image->materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &s_DefaultColor);
        world->AddChild(m_Content, m_Root);

        auto innerImageEntity = world->CreateEntity(editor->GetEditorScene(), "Border");
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        auto innerMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/editor_darkbg.sass");
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(m_Root, innerImageEntity);

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

        m_Label = world->CreateEntity(editor->GetEditorScene(), "Title");
        world->AddComponent<WidgetComponent>(m_Label);
        auto text = world->AddComponent<TextComponent>(m_Label);
        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 14;
        if (m_Value->IsSet())
        {
            text->text = GetAssetName(m_Value->GetAssetPath());
        }
        else
        {
            text->text = "None";
        }
        auto textRect = world->AddComponent<RectTransformComponent>(m_Label);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        textRect->minX = iconSize + borderSize * 3;
        textRect->minY = borderSize + 8;
        textRect->maxY = borderSize;
        world->AddChild(innerImageEntity, m_Label);
    }

    template<typename T>
    void AssetReferenceEditor<T>::Update()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto* inputComp = world->GetSingletonComponent<input::InputComponent>();
        auto* dragDropState = world->GetSingletonComponent<singleton_components::DragDropStateComponent>();

        auto image = world->GetComponent<ImageComponent>(m_Root);


        if (!dragDropState->dragDropAsset)
        {
            if (m_IsHighlighted)
            {
                image->materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &s_DefaultColor);
            }

            return;
        }

        auto* rootRect = world->GetComponent<RectTransformComponent>(m_Root);
        if (rootRect->rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY)))
        {
            bool isValid = dragDropState->dragDropAsset->GetReflectType() == T::GetReflection();
            if (!m_IsHighlighted)
            {
                m_IsHighlighted = true;
                if (isValid)
                {
                    image->materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &s_GreenColor);
                }
                else
                {
                    image->materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &s_RedColor);
                }
            }

            if (isValid)
            {
                if (inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] != se::input::KeyState::Down)
                {
                    m_Value->Set(dragDropState->dragDropAsset->m_Path);

                    auto* text = world->GetComponent<TextComponent>(m_Label);
                    text->text = GetAssetName(m_Value->GetAssetPath());
                }
            }
        }
        else if (m_IsHighlighted)
        {
            image->materialInstance->SetUniform("uniform_color", asset::shader::ast::AstType::Vec3, 1, &s_DefaultColor);
            m_IsHighlighted = false;
        }
    }
}

#include "se_editor_ui_properties_AssetReferenceEditor.generated.h"
