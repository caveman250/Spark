#include <editor/ui/properties/util/PropertyUtil.h>
#include "PtrEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("*", PtrEditor)
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::shared_ptr<>", PtrEditor)

    void PtrEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_Type = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void PtrEditor::ConstructUI(const PropertyEditorParams& params)
    {
        auto containedType = m_Type->GetContainedValueType(m_Value);
        auto wrappedParams = params;
        wrappedParams.type = containedType;
        wrappedParams.value = m_Type->GetContainedValue(m_Value);
        m_WrappedEditor = CreatePropertyEditor(wrappedParams);

        m_TitleMode = params.titleModeOverride != PropertyTitleMode::None ?
            params.titleModeOverride :
            GetDefaultTitleMode();

        if (!m_WrappedEditor)
        {
            PropertyEditor::ConstructUI(params);

            auto app = Application::Get();
            auto world = app->GetWorld();
            auto editor = app->GetEditor();

            auto entity = world->CreateEntity(editor->GetEditorScene(), params.name);
            auto rect = world->AddComponent<RectTransformComponent>(entity);
            rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            rect->minX = 5;
            rect->maxX = 15;
            world->AddComponent<WidgetComponent>(entity);
            world->AddChild(m_Content, entity);

            auto titleMode = m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : GetDefaultTitleMode();
            auto text = properties::util::CreateMissingPropertyEditorText(containedType,
                                                                          titleMode == PropertyTitleMode::Inline ? 0.35f : 1.f,
                                                                          0);
            world->AddChild(entity, text);
        }
    }

    void PtrEditor::Update()
    {
        m_WrappedEditor->Update();
    }

    ecs::Id PtrEditor::GetTitleId() const
    {
        return m_WrappedEditor->GetTitleId();
    }

    void PtrEditor::BeginRename(const std::string_view editText,
                                const std::function<void(const std::string&, EditableTextComponent*)>& onComitted,
                                const std::function<void(EditableTextComponent*)>& onCancelled)
    {
        m_WrappedEditor->BeginRename(editText, onComitted, onCancelled);
    }

    void PtrEditor::DestroyUI()
    {
        m_WrappedEditor->DestroyUI();
    }
}
