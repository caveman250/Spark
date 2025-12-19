#pragma once

#include "engine/Application.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/shader/Shader.h"
#include "engine/render/Material.h"
#include "engine/ui/util/EditableTextUtil.h"

namespace se::editor::ui::properties
{
    template <typename S>
    class StringEditor : public PropertyEditor
    {
        SPARK_CLASS_TEMPLATED()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        void Update() override;

    private:
        S* m_Value = nullptr;
        ecs::Id m_Label;
    };

    SPARK_INSTANTIATE_TEMPLATE(StringEditor, std::string);

    template <typename S>
    void StringEditor<S>::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<S*>(value);
    }

    template <typename S>
    void StringEditor<S>::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        constexpr int fontSize = 14;
        constexpr int padding = 4;
        constexpr int textYOffset = padding / 2;
        constexpr int borderSize = 2;

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity("String Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.35f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = ariel->GetLineHeight(fontSize) + padding + borderSize * 2 + 0.5f;

        world->AddComponent<WidgetComponent>(bg);
        auto image = world->AddComponent<ImageComponent>(bg);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = render::Material::CreateMaterial({vert}, {frag});
            material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(m_Content, bg);

        auto innerImageEntity = world->CreateEntity("Border", true);
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        static std::shared_ptr<render::Material> innerMaterial = nullptr;
        if (!innerMaterial)
        {
            innerMaterial = render::Material::CreateMaterial({vert}, {frag});
            innerMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }
        innerImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(innerMaterial);
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = borderSize;
        world->AddChild(bg, innerImageEntity);

        EditableTextComponent* editableText = nullptr;
        m_Label = se::ui::util::CreateEditableText(world, ariel, fontSize, &editableText);
        editableText->text = std::format("{}", *m_Value);
        std::function cb = [this](std::string newVal)
        {
            *m_Value = newVal.data();
        };
        editableText->onComitted.Subscribe(std::move(cb));

        auto labelRect = world->AddComponent<RectTransformComponent>(m_Label);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        labelRect->minX = 2;
        labelRect->maxX = 2;
        labelRect->minY = textYOffset;
        world->AddChild(innerImageEntity, m_Label);
    }

    template <typename S>
    void StringEditor<S>::Update()
    {
        if (auto text = Application::Get()->GetWorld()->GetComponent<EditableTextComponent>(m_Label))
        {
            text->text = std::format("{}", *m_Value);
        }
    }
}

#include "se_editor_ui_properties_StringEditor.generated.h"
