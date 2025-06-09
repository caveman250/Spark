#include "Vec3Editor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(math::Vec3, Vec3Editor, Vec3);

    void Vec3Editor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<math::Vec3*>(value);
    }

    void Vec3Editor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        auto world = Application::Get()->GetWorld();

        auto box = world->CreateEntity("Vect3 Editor Box", true);
        auto* boxRect = world->AddComponent<se::ui::components::RectTransformComponent>(box);
        world->AddComponent<WidgetComponent>(box);
        boxRect->anchors = { constructTitle ? 0.35f : 0.f, 1.f, 0.f, 1.f };
        world->AddChild(m_Content, box);

        m_XEditor.SetValue(&m_Value->x, nullptr);
        m_XEditor.ConstructUI("X", false, { 0.f, 0.33f, 0.f, 1.f }, false, false);
        m_XEditor.GetRectTransform()->minX = 2;
        m_XEditor.GetRectTransform()->maxX = -2;
        world->AddChild(box, m_XEditor.GetWidgetId());

        m_YEditor.SetValue(&m_Value->x, nullptr);
        m_YEditor.ConstructUI("Y", false, { 0.33f, 0.66f, 0.f, 1.f }, false, false);
        m_YEditor.GetRectTransform()->minX = 2;
        m_YEditor.GetRectTransform()->maxX = 2;
        world->AddChild(box, m_YEditor.GetWidgetId());

        m_ZEditor.SetValue(&m_Value->z, nullptr);
        m_ZEditor.ConstructUI("Z", false, { 0.66f, 1.f, 0.f, 1.f }, false, false);
        m_ZEditor.GetRectTransform()->minX = -2;
        m_ZEditor.GetRectTransform()->maxX = 2;
        world->AddChild(box, m_ZEditor.GetWidgetId());
    }

    void Vec3Editor::Update()
    {
        m_ZEditor.SetValue(&m_Value->x, nullptr);
        m_XEditor.Update();
        m_YEditor.SetValue(&m_Value->y, nullptr);
        m_YEditor.Update();
        m_ZEditor.SetValue(&m_Value->z, nullptr);
        m_ZEditor.Update();
    }
}
