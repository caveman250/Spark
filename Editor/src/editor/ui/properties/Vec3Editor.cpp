#include "Vec3Editor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(math::Vec3, Vec3Editor, Vec3);

    DEFINE_SPARK_CLASS_BEGIN(Vec3Editor)
    DEFINE_SPARK_CLASS_END(Vec3Editor)

    void Vec3Editor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<math::Vec3*>(value);
    }

    void Vec3Editor::ConstructUI(const String& name, bool constructTitle)
    {
        PropertyEditor::ConstructUI(name, constructTitle);

        auto world = Application::Get()->GetWorld();

        m_XEditor.SetValue(&m_Value->x, nullptr);
        m_XEditor.ConstructUI("X", false);
        m_XEditor.GetRectTransform()->minY = 22;
        m_XEditor.GetRectTransform()->maxY = 52;
        m_XEditor.GetRectTransform()->minX = 2;
        m_XEditor.GetRectTransform()->maxX = -2;
        m_XEditor.GetRectTransform()->anchors = { 0.f, 0.33f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_XEditor.GetWidgetId());

        m_YEditor.SetValue(&m_Value->x, nullptr);
        m_YEditor.ConstructUI("Y", false);
        m_YEditor.GetRectTransform()->minY = 22;
        m_YEditor.GetRectTransform()->maxY = 52;
        m_YEditor.GetRectTransform()->minX = 2;
        m_YEditor.GetRectTransform()->maxX = 2;
        m_YEditor.GetRectTransform()->anchors = { 0.33f, 0.66f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_YEditor.GetWidgetId());

        m_ZEditor.SetValue(&m_Value->z, nullptr);
        m_ZEditor.ConstructUI("Z", false);
        m_ZEditor.GetRectTransform()->minY = 22;
        m_ZEditor.GetRectTransform()->maxY = 52;
        m_ZEditor.GetRectTransform()->minX = -2;
        m_ZEditor.GetRectTransform()->maxX = 2;
        m_ZEditor.GetRectTransform()->anchors = { 0.66f, 1.f, 0.f, 0.f };
        world->AddChild(m_WidgetId, m_ZEditor.GetWidgetId());

        m_RectTransform->maxY = 52;
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
