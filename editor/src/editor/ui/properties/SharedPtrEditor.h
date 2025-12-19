#pragma once

#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class SharedPtrEditor : public PropertyEditor
    {
    SPARK_CLASS()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        void Update() override;
        PropertyTitleMode GetTitleMode() const override { return m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : PropertyEditor::GetTitleMode(); }
        ecs::Id GetWidgetId() const override { return m_WrappedEditor ? m_WrappedEditor->GetWidgetId() : PropertyEditor::GetWidgetId(); }
    private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_Type = nullptr;
        PropertyEditor* m_WrappedEditor = nullptr;
    };
}
