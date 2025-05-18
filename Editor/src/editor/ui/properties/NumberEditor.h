#pragma once

#include "engine/Application.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "inttypes.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/util/EditableTextUtil.h"

namespace se::editor::ui::properties
{
    template <typename T>
    concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template <Number N>
    class NumberEditor : public PropertyEditor
    {
        SPARK_CLASS_TEMPLATED(NumberEditor, N)

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors) override;
        void Update() override;

    private:
        N* m_Value = nullptr;
        ecs::Id m_Label;
    };

    DEFINE_SPARK_CLASS_TEMPLATED_BEGIN(NumberEditor, TEMPLATE_TYPES(N), TEMPLATE_PARAMETERS(Number N))
    DEFINE_SPARK_CLASS_TEMPLATED_END(NumberEditor)

    template <Number N>
    void NumberEditor<N>::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<N*>(value);
    }

    template <Number N>
    void NumberEditor<N>::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors);

        constexpr int fontSize = 14;
        constexpr int padding = 4;
        constexpr int textYOffset = padding / 2;
        constexpr int borderSize = 2;

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");

        auto bg = world->CreateEntity("Int Editor", true);
        auto bgTransform = world->AddComponent<RectTransformComponent>(bg);
        bgTransform->anchors = { .left = constructTitle ? 0.3f : 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
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
        std::function cb = [this](String newVal)
        {
            N i;
            if constexpr (std::is_same_v<N, char>)
            {
                if (SSCANF(newVal.Data(), "%c", &i, 1) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int8_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIi8, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint8_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIu8, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int16_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIi16, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint16_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIu16, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int32_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIi32, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint32_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIu32, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, int64_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIi64, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, uint64_t>)
            {
                if (SSCANF(newVal.Data(), "%" PRIu64, &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, float>)
            {
                if (SSCANF(newVal.Data(), "%f", &i) == 1)
                {
                    *m_Value = i;
                }
            }
            else if constexpr (std::is_same_v<N, double>)
            {
                if (SSCANF(newVal.Data(), "%lf", &i) == 1)
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
