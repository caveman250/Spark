#include "Vector3Editor.h"

#include <engine/Application.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/TextComponent.h>
#include <engine/ui/components/WidgetComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(math::Vec3, Vec3Editor);

    DEFINE_SPARK_CLASS_BEGIN(Vec3Editor)
    DEFINE_SPARK_CLASS_END()

    void Vec3Editor::SetValue(void* value)
    {
        m_Value = static_cast<math::Vec3*>(value);
    }

    void Vec3Editor::ConstructUI()
    {
        auto world = Application::Get()->GetWorld();
        auto ariel = asset::AssetManager::Get()->GetAsset<asset::Font>("/builtin_assets/fonts/Arial.sass");

        m_WidgetId = world->CreateEntity("Vec3 Editor");
        m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
        m_RectTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        m_RectTransform->minX = 5;
        m_RectTransform->maxY = 36;
        world->AddComponent<WidgetComponent>(m_WidgetId);

        auto titleEntity = world->CreateEntity("Title");
        auto titleText = world->AddComponent<TextComponent>(titleEntity);
        titleText->font = ariel;
        titleText->fontSize = 21;
        titleText->text = m_Name;
        auto titleRect = world->AddComponent<RectTransformComponent>(titleEntity);
        titleRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        titleRect->minY = 0;
        titleRect->maxY = 18;
        world->AddChild(m_WidgetId, titleEntity);

        m_Label = world->CreateEntity("Label");
        auto text = world->AddComponent<TextComponent>(m_Label);
        text->font = ariel;
        text->fontSize = 16;
        text->text = std::format("X: {:.2f}, Y: {:.2f}, Z: {:.2f}", m_Value->x, m_Value->y, m_Value->z);
        auto labelRect = world->AddComponent<RectTransformComponent>(m_Label);
        labelRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        labelRect->minY = 18;
        labelRect->maxY = 36;
        world->AddChild(m_WidgetId, m_Label);
    }

    void Vec3Editor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    void Vec3Editor::Update()
    {
        auto text = Application::Get()->GetWorld()->GetComponent<TextComponent>(m_Label);
        text->text = std::format("X: {:.2f}, Y: {:.2f}, Z: {:.2f}", m_Value->x, m_Value->y, m_Value->z);
    }
}
