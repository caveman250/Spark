#pragma once

#include "PropertyEditor.h"
#include "spark.h"
#include "editor/Transactions.h"
#include "editor/singleton_components/DragDropStateComponent.h"
#include "engine/Application.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/mesh/Model.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ecs/SceneSaveData.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/util/ContextMenuUtil.h"
#include "editor/Transactions.h"

namespace se::editor::ui::properties
{
    template <typename T>
    class AssetReferenceEditor : public PropertyEditor
    {
        SPARK_CLASS_TEMPLATED()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;

    private:
        void SetupIconAndText(TextComponent* text, ImageComponent* image);

        asset::AssetReference<T>* m_Value = nullptr;
        asset::AssetReference<T> m_OldVal = {};
        ecs::Id m_Root = {};
        ecs::Id m_Label = {};
        ecs::Id m_Icon = {};
        bool m_IsHighlighted = false;

        inline static const math::Vec4 s_DefaultColor = math::Vec4(0.6f, 0.6f, 0.6f, 1.f);
        inline static const math::Vec4 s_GreenColor = math::Vec4(0.f, 1.f, 0.f, 1.f);
        inline static const math::Vec4 s_RedColor = math::Vec4(1.f, 0.f, 0.f, 1.f);
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
        m_OldVal = *m_Value;
    }

    inline std::string GetAssetName(const std::string& path)
    {
        std::string smallPath = path;
        auto lastSlashIt = smallPath.find_last_of('/');
        auto extensionIt = smallPath.find_last_of('.');
        return smallPath.substr(lastSlashIt + 1, extensionIt - 1 - lastSlashIt);
    }

    template<typename T>
    void AssetReferenceEditor<T>::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        constexpr int iconSize = 32;
        constexpr int borderSize = 2;

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/CascadiaCode.sass");

        m_Root = world->CreateEntity(editor->GetEditorScene(), "Asset Reference Editor");
        auto bgTransform = world->AddComponent<RectTransformComponent>(m_Root);
        bgTransform->anchors = { .left = params.constructTitle ? 0.35f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = iconSize + borderSize * 4;
        world->AddComponent<WidgetComponent>(m_Root);
        auto image = world->AddComponent<ImageComponent>(m_Root);
        auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_runtime_flat_color.sass");
        image->materialInstance = std::make_shared<render::MaterialInstance>(material);
        image->materialInstance->SetUniform("uniform_color", 1, &s_DefaultColor);
        world->AddChild(m_Content, m_Root);

        auto innerImageEntity = world->CreateEntity(editor->GetEditorScene(), "Border");
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        auto innerMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/editor_darkbg.sass");
        innerImage->materialInstance = std::make_shared<render::MaterialInstance>(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(m_Root, innerImageEntity);

        m_Icon = world->CreateEntity(editor->GetEditorScene(), "Icon");
        auto iconImage = world->AddComponent<ImageComponent>(m_Icon);
        auto iconMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
        iconImage->materialInstance = std::make_shared<render::MaterialInstance>(iconMaterial);
        auto iconTransform = world->AddComponent<RectTransformComponent>(m_Icon);
        iconTransform->anchors = { .left = 0.f, .right = 0.f, .top = 0.f, .bottom = 0.f };
        iconTransform->minX = iconTransform->minY = borderSize;
        iconTransform->maxX = iconTransform->maxY = borderSize + iconSize;
        world->AddChild(innerImageEntity, m_Icon);

        m_Label = world->CreateEntity(editor->GetEditorScene(), "Title");
        world->AddComponent<WidgetComponent>(m_Label);
        auto text = world->AddComponent<TextComponent>(m_Label);
        text->font = "/engine_assets/fonts/CascadiaCode.sass";
        text->fontSize = 14;
        text->wrap = se::ui::text::WrapMode::Crop;
        SetupIconAndText(text, iconImage);
        auto textRect = world->AddComponent<RectTransformComponent>(m_Label);
        textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        textRect->minX = iconSize + borderSize * 3;
        textRect->minY = borderSize + 8;
        textRect->maxY = borderSize;
        world->AddChild(innerImageEntity, m_Label);

        auto* input = world->GetSingletonComponent<input::InputComponent>();
        auto button = world->CreateEntity(editor->GetEditorScene(), "Button");
        auto* buttonTransform = world->AddComponent<RectTransformComponent>(button);
        buttonTransform->anchors = { 0.f, 1.f, 0.f, 1.f };
        auto* buttonComp = world->AddComponent<ButtonComponent>(button);
        buttonComp->onReleased.Subscribe([input, value = m_Value](input::MouseButton button, bool)
        {
            if (button == input::MouseButton::Right)
            {
                se::ui::util::ContextMenuParams params = {
                    .fontSize = 14,
                    .mousePos = { input->mouseX, input->mouseY },
                    .scene = Application::Get()->GetEditor()->GetEditorScene()
                };
                params.AddOption("Reset", [value]()
                {
                    auto oldVal = *value;
                    Transactions::Get()->PushAction([value]()
                    {
                        value->Reset();
                    },
                    [value, oldVal]()
                    {
                        *value = oldVal;
                    });
                });
                se::ui::util::CreateContextMenu(params);
            }
        });
        auto* buttonWidget = world->AddComponent<WidgetComponent>(button);
        buttonWidget->visibility = se::ui::Visibility::Hidden;
        world->AddChild(m_Root, button);
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
                image->materialInstance->SetUniform("uniform_color", 1, &s_DefaultColor);
            }
        }
        else
        {
            auto* rootRect = world->GetComponent<RectTransformComponent>(m_Root);
            if (rootRect->rect.Contains(math::IntVec2(inputComp->mouseX, inputComp->mouseY)))
            {
                bool isValid = dragDropState->dragDropAsset->GetReflectType() == T::GetReflection();
                if (!m_IsHighlighted)
                {
                    m_IsHighlighted = true;
                    if (isValid)
                    {
                        image->materialInstance->SetUniform("uniform_color", 1, &s_GreenColor);
                    }
                    else
                    {
                        image->materialInstance->SetUniform("uniform_color", 1, &s_RedColor);
                    }
                }

                if (isValid)
                {
                    if (inputComp->mouseButtonStates[static_cast<int>(se::input::MouseButton::Left)] != se::input::KeyState::Down)
                    {
                        std::string currentAsset = m_Value->GetAssetPath();
                        std::string newAsset = dragDropState->dragDropAsset->m_Path;
                        Transactions::Get()->PushAction([this, newAsset]()
                        {
                            m_Value->Set(newAsset);
                        },
                        [currentAsset, this]()
                        {
                            m_Value->Set(currentAsset);
                        });
                    }
                }
            }
            else if (m_IsHighlighted)
            {
                image->materialInstance->SetUniform("uniform_color", 1, &s_DefaultColor);
                m_IsHighlighted = false;
            }
        }

        if (m_Value && *m_Value != m_OldVal)
        {
            auto* iconImage = world->GetComponent<ImageComponent>(m_Icon);
            auto* text = world->GetComponent<TextComponent>(m_Label);
            SetupIconAndText(text, iconImage);
            m_OldVal = *m_Value;
        }
    }

    template<typename T>
    void AssetReferenceEditor<T>::SetupIconAndText(TextComponent* text,
        ImageComponent* image)
    {
        if (m_Value->IsSet())
        {
            text->text = GetAssetName(m_Value->GetAssetPath());
            const auto fileImage = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_file.sass");
            image->materialInstance->SetUniform("Texture", 1, &fileImage);
        }
        else
        {
            text->text = "None";
            const auto fileImage = asset::AssetReference<asset::Texture>("/engine_assets/textures/no_file.sass");
            image->materialInstance->SetUniform("Texture", 1, &fileImage);
        }
    }
}

#include "se_editor_ui_properties_AssetReferenceEditor.generated.h"
