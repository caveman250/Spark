#include <editor/ui/properties/util/PropertyUtil.h>
#include "SharedPtrEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("*", SharedPtrEditor)
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::shared_ptr<>", SharedPtrEditor)

    void SharedPtrEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_Type = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void SharedPtrEditor::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        auto containedType = m_Type->GetContainedValueType(m_Value);
        m_WrappedEditor = CreatePropertyEditor(name,
                                               containedType,
                                               m_Type->GetContainedValue(m_Value),
                                               anchors,
                                               collapsed,
                                               withBackground,
                                               constructTitle);

        if (!m_WrappedEditor)
        {
            PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

            auto world = Application::Get()->GetWorld();

            auto entity = world->CreateEntity(name, true);
            auto rect = world->AddComponent<RectTransformComponent>(entity);
            rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            rect->minX = 5;
            rect->maxX = 15;
            world->AddComponent<WidgetComponent>(entity);
            world->AddChild(m_Content, entity);

            auto titleMode = m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : PropertyTitleMode::Inline;
            auto text = properties::util::CreateMissingPropertyEditorText(containedType,
                                                                          titleMode == PropertyTitleMode::Inline ? 0.35f : 1.f,
                                                                          0);
            world->AddChild(entity, text);
        }
    }

    void SharedPtrEditor::Update()
    {
    }
}
