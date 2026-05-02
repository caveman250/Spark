#pragma once

#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class PtrEditor : public PropertyEditor
    {
    SPARK_CLASS()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;
        PropertyTitleMode GetDefaultTitleMode() const override { return m_WrappedEditor ? m_WrappedEditor->GetDefaultTitleMode() : PropertyEditor::GetDefaultTitleMode(); }
        ecs::Id GetWidgetId() const override { return m_WrappedEditor ? m_WrappedEditor->GetWidgetId() : PropertyEditor::GetWidgetId(); }
        void BeginRename(const std::string_view editText,
                         const std::function<void(const std::string&, EditableTextComponent*)>& onComitted,
                         const std::function<void(EditableTextComponent*)>& onCancelled) override;
        void DestroyUI() override;

private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_Type = nullptr;
        PropertyEditor* m_WrappedEditor = nullptr;
    };
}
