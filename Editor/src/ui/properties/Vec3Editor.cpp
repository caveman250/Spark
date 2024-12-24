#include "Vec3Editor.h"

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
        m_RectTransform->maxX = 5;
        m_RectTransform->maxY = 54;
        world->AddComponent<WidgetComponent>(m_WidgetId);

        auto titleEntity = world->CreateEntity("Title");
        auto titleText = world->AddComponent<TextComponent>(titleEntity);
        titleText->font = ariel;
        titleText->fontSize = 18;
        titleText->text = m_Name;
        auto titleRect = world->AddComponent<RectTransformComponent>(titleEntity);
        titleRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        titleRect->minX = 2;
        titleRect->maxX = 2;
        titleRect->minY = 0;
        titleRect->maxY = 18;
        world->AddChild(m_WidgetId, titleEntity);

        m_XEditor.SetValue(&m_Value->x);
        m_XEditor.ConstructUI();
        m_XEditor.GetRectTransform()->minX = 2;
        m_XEditor.GetRectTransform()->maxX = -2;
        m_XEditor.GetRectTransform()->minY = 22;
        m_XEditor.GetRectTransform()->maxY = 52;
        m_XEditor.GetRectTransform()->anchors = { 0.f, 0.33f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_XEditor.GetWidgetId());

        m_YEditor.SetValue(&m_Value->x);
        m_YEditor.ConstructUI();
        m_YEditor.GetRectTransform()->minX = 2;
        m_YEditor.GetRectTransform()->maxX = 2;
        m_YEditor.GetRectTransform()->minY = 22;
        m_YEditor.GetRectTransform()->maxY = 52;
        m_YEditor.GetRectTransform()->anchors = { 0.33f, 0.66f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_YEditor.GetWidgetId());

        m_ZEditor.SetValue(&m_Value->z);
        m_ZEditor.ConstructUI();
        m_ZEditor.GetRectTransform()->minX = -2;
        m_ZEditor.GetRectTransform()->maxX = 2;
        m_ZEditor.GetRectTransform()->minY = 22;
        m_ZEditor.GetRectTransform()->maxY = 52;
        m_ZEditor.GetRectTransform()->anchors = { 0.66f, 1.f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_ZEditor.GetWidgetId());
    }

    void Vec3Editor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    void Vec3Editor::Update()
    {
        m_ZEditor.SetValue(&m_Value->x);
        m_XEditor.Update();
        m_YEditor.SetValue(&m_Value->y);
        m_YEditor.Update();
        m_ZEditor.SetValue(&m_Value->z);
        m_ZEditor.Update();
    }
}
