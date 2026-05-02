#include <editor/ui/properties/util/PropertyUtil.h>
#include "SingleMemberEditor.h"

#include "engine/Application.h"
#include "engine/render/UniformStorage.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/asset/shader/ShaderSettings.h"
namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::render::UniformStorage, SingleMemberEditor, UniformStorage)
    DEFINE_PROPERTY_EDITOR(se::ShaderSettings, SingleMemberEditor, ShaderSettings)
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<int>, SingleMemberEditor, ShaderSettingDefinition_int)
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<float>, SingleMemberEditor, ShaderSettingDefinition_float)
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<math::Vec3>, SingleMemberEditor, ShaderSettingDefinition_Vec3)
    DEFINE_PROPERTY_EDITOR(se::ShaderSettingDefinition<math::Vec4>, SingleMemberEditor, ShaderSettingDefinition_Vec4)

    void SingleMemberEditor::SetValue(void* value, const reflect::Type* type)
    {
        m_Value = value;
        m_Type = static_cast<const reflect::Class*>(type);
    }

    void SingleMemberEditor::ConstructUI(const PropertyEditorParams& params)
    {
        auto wrappedParams = params;
        wrappedParams.type = m_Type->members[0].type;
        wrappedParams.value = m_Type->members[0].get(m_Value);
        m_WrappedEditor = CreatePropertyEditor(wrappedParams);

        if (!m_WrappedEditor)
        {
            PropertyEditor::ConstructUI(params);

            auto app = Application::Get();
            auto world = app->GetWorld();
            auto editor = app->GetEditorRuntime();

            auto entity = world->CreateEntity(editor->GetEditorScene(), params.name);
            auto rect = world->AddComponent<RectTransformComponent>(entity);
            rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            rect->minX = 5;
            rect->maxX = 15;
            world->AddComponent<WidgetComponent>(entity);
            world->AddChild(m_Content, entity);

            auto titleMode = m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : GetDefaultTitleMode();
            auto text = properties::util::CreateMissingPropertyEditorText(m_Type->members[0].type,
                                                                          titleMode == PropertyTitleMode::Inline ? 0.35f : 1.f,
                                                                          0);
            world->AddChild(entity, text);
        }
    }

    void SingleMemberEditor::Update()
    {
        m_WrappedEditor->Update();
    }

    PropertyTitleMode SingleMemberEditor::GetDefaultTitleMode() const
    {
        return m_WrappedEditor->GetDefaultTitleMode();
    }

    ecs::Id SingleMemberEditor::GetWidgetId() const
    {
        return m_WrappedEditor->GetWidgetId();
    }

    ecs::Id SingleMemberEditor::GetTitleId() const
    {
        return m_WrappedEditor->GetTitleId();
    }

    void SingleMemberEditor::DestroyUI()
    {
        m_WrappedEditor->DestroyUI();
    }
}
