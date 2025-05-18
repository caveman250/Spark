#pragma once

#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class SharedPtrEditor : public PropertyEditor
    {
    SPARK_CLASS(SharedPtrEditor)

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors) override;
        void Update() override;
        PropertyTitleMode::Type GetTitleMode() const override { return m_WrappedEditor ? m_WrappedEditor->GetTitleMode() : PropertyEditor::GetTitleMode(); }

    private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_Type = nullptr;
        PropertyEditor* m_WrappedEditor = nullptr;
    };
}
