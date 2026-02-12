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

    void Vec3Editor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        auto box = world->CreateEntity(editor->GetEditorScene(), "Vect3 Editor Box");
        auto* boxRect = world->AddComponent<se::ui::components::RectTransformComponent>(box);
        world->AddComponent<WidgetComponent>(box);
        boxRect->anchors = { params.constructTitle ? 0.35f : 0.f, 1.f, 0.f, 1.f };
        world->AddChild(m_Content, box);

        m_XEditor.SetValue(&m_Value->x, nullptr);
        m_XEditor.ConstructUI(PropertyEditorParams("X",
            reflect::TypeResolver<float>::get(),
            &m_Value->x,
            { 0.f, 0.33f, 0.f, 1.f },
            false,
            false,
            true,
            {}));
        m_XEditor.GetRectTransform()->minX = 2;
        m_XEditor.GetRectTransform()->maxX = -2;
        world->AddChild(box, m_XEditor.GetWidgetId());

        m_YEditor.SetValue(&m_Value->x, nullptr);
        m_YEditor.ConstructUI(PropertyEditorParams("Y",
           reflect::TypeResolver<float>::get(),
           &m_Value->y,
           { 0.33f, 0.66f, 0.f, 1.f },
           false,
           false,
           true,
           {}));
        m_YEditor.GetRectTransform()->minX = 2;
        m_YEditor.GetRectTransform()->maxX = 2;
        world->AddChild(box, m_YEditor.GetWidgetId());

        m_ZEditor.SetValue(&m_Value->z, nullptr);
        m_ZEditor.ConstructUI(PropertyEditorParams("Z",
           reflect::TypeResolver<float>::get(),
           &m_Value->z,
           { 0.66f, 1.f, 0.f, 1.f },
           false,
           false,
           true,
           {}));
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
