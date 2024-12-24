#include "FloatEditor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(float, FloatEditor);

    DEFINE_SPARK_CLASS_BEGIN(FloatEditor)
    DEFINE_SPARK_CLASS_END()

    void FloatEditor::SetValue(void* value)
    {
        m_Value = static_cast<float*>(value);
    }

    void FloatEditor::ConstructUI()
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        auto ariel = assetManager->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");

        m_WidgetId = world->CreateEntity("Float Editor");
        m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
        m_RectTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        m_RectTransform->maxY = 36;
        world->AddComponent<WidgetComponent>(m_WidgetId);
        auto image = world->AddComponent<ImageComponent>(m_WidgetId);
        auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/flat_color.sass");
        image->material = render::Material::CreateMaterial({vert}, {frag});
        image->material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.6f, 0.6f, 0.6f));

        auto innerImageEntity = world->CreateEntity("Border");
        auto innerImage = world->AddComponent<ImageComponent>(innerImageEntity);
        innerImage->material = render::Material::CreateMaterial({vert}, {frag});
        innerImage->material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        auto innerTransform = world->AddComponent<RectTransformComponent>(innerImageEntity);
        innerTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        innerTransform->minX = innerTransform->maxX = innerTransform->minY = innerTransform->maxY = 2.f;
        world->AddChild(m_WidgetId, innerImageEntity);

        m_Label = world->CreateEntity("Label");
        auto text = world->AddComponent<TextComponent>(m_Label);
        text->font = ariel;
        text->fontSize = 16;
        text->text = std::format("{:.2f}", *m_Value);
        auto labelRect = world->AddComponent<RectTransformComponent>(m_Label);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        labelRect->minY = 2;
        labelRect->maxY = 2;
        labelRect->minX = 2;
        labelRect->maxX = 2;
        world->AddChild(innerImageEntity, m_Label);
    }

    void FloatEditor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    void FloatEditor::Update()
    {
        auto text = Application::Get()->GetWorld()->GetComponent<TextComponent>(m_Label);
        text->text = std::format("{:.2f}", *m_Value);
    }
}
