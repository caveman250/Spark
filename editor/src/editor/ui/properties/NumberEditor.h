#pragma once

#include "engine/Application.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "inttypes.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/shader/Shader.h"
#include "engine/render/Material.h"
#include "engine/ui/util/EditableTextUtil.h"

namespace se::editor::ui::properties
{
    template <typename T>
    concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template <Number N>
    class NumberEditor : public PropertyEditor
    {
        SPARK_CLASS_TEMPLATED()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        void Update() override;

    private:
        N* m_Value = nullptr;
        ecs::Id m_Label;
    };

    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, char);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, int8_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, uint8_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, int16_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, uint16_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, int32_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, uint32_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, int64_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, uint64_t);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, float);
    SPARK_INSTANTIATE_TEMPLATE(NumberEditor, double);

    template <Number N>
    void NumberEditor<N>::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<N*>(value);
    }

    template <Number N>
    void NumberEditor<N>::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
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

        auto bg = world->CreateEntity(editor->GetEditorScene(), "Int Editor");
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.35f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        bgTransform->minY = 0;
        bgTransform->maxY = ariel->GetLineHeight(fontSize) + padding + borderSize * 2 + 0.5f;

        world->AddComponent<WidgetComponent>(bg);
        auto image = world->AddComponent<ImageComponent>(bg);

        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));
        }
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        world->AddChild(m_Content, bg);

        auto innerImageEntity = world->CreateEntity(editor->GetEditorScene(), "Border");
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
        world->AddChild(bg, innerImageEntity);

        EditableTextComponent* editableText = nullptr;
        m_Label = se::ui::util::CreateEditableText(world, ariel, fontSize, &editableText);
        editableText->text = std::format("{}", *m_Value);
        std::function cb = [this](std::string newVal)
        {
            N i;
            if constexpr (std::is_same_v<N, char>)
            {
                if (SSCANF(newVal.data(), "%c", &i, 1) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int8_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIi8, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint8_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIu8, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int16_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIi16, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint16_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIu16, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int32_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIi32, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint32_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIu32, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int64_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIi64, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint64_t>)
            {
                if (SSCANF(newVal.data(), "%" PRIu64, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, float>)
            {
                if (SSCANF(newVal.data(), "%f", &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, double>)
            {
                if (SSCANF(newVal.data(), "%lf", &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else
            {
                static_assert(false);
            }
        };
        editableText->onComitted.Subscribe(std::move(cb));

        auto labelRect = world->AddComponent<RectTransformComponent>(m_Label);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        labelRect->minX = 2;
        labelRect->maxX = 2;
        labelRect->minY = textYOffset;
        world->AddChild(innerImageEntity, m_Label);
    }

    template <Number N>
    void NumberEditor<N>::Update()
    {
        if (auto text = Application::Get()->GetWorld()->GetComponent<EditableTextComponent>(m_Label))
        {
            if constexpr (std::is_floating_point_v<N>)
            {
                text->text = std::format("{:.2f}", *m_Value);
            }
            else
            {
                text->text = std::format("{}", *m_Value);
            }
        }
    }
}

#include "se_editor_ui_properties_NumberEditor.generated.h"
