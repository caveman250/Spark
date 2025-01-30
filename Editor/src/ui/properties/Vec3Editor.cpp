#include "Vec3Editor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(math::Vec3, Vec3Editor);

    DEFINE_SPARK_CLASS_BEGIN(Vec3Editor)
    DEFINE_SPARK_CLASS_END()

    void Vec3Editor::SetValue(void* value)
    {
        m_Value = static_cast<math::Vec3*>(value);
    }

    void Vec3Editor::ConstructUI(const String& name, bool constructTitle)
    {
        PropertyEditor::ConstructUI(name, constructTitle);

        auto world = Application::Get()->GetWorld();

        m_XEditor.SetValue(&m_Value->x);
        m_XEditor.ConstructUI("X", false);
        m_XEditor.GetRectTransform()->minX = 2;
        m_XEditor.GetRectTransform()->maxX = -2;
        m_XEditor.GetRectTransform()->anchors = { 0.f, 0.33f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_XEditor.GetWidgetId());

        m_YEditor.SetValue(&m_Value->x);
        m_YEditor.ConstructUI("Y", false);
        m_YEditor.GetRectTransform()->minX = 2;
        m_YEditor.GetRectTransform()->maxX = 2;
        m_YEditor.GetRectTransform()->anchors = { 0.33f, 0.66f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_YEditor.GetWidgetId());

        m_ZEditor.SetValue(&m_Value->z);
        m_ZEditor.ConstructUI("Z", false);
        m_ZEditor.GetRectTransform()->minX = -2;
        m_ZEditor.GetRectTransform()->maxX = 2;
        m_ZEditor.GetRectTransform()->anchors = { 0.66f, 1.f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_ZEditor.GetWidgetId());

        m_RectTransform->maxY = 52;
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
