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

        constexpr int fontSize = 14;
        constexpr int padding = 4;
        constexpr int textYOffset = padding / 2;
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
        bgTransform->maxY = 40;

        world->AddComponent<WidgetComponent>(bg);
        auto image = world->AddComponent<ImageComponent>(bg);

        auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_lightbg.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(m_Content, bg);
    }

    template<typename T>
    void AssetReferenceEditor<T>::Update()
    {
    }
}

#include "se_editor_ui_properties_AssetReferenceEditor.generated.h"
