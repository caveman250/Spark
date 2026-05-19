#pragma once

#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class ClassEditor : public PropertyEditor
    {
    SPARK_CLASS()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        PropertyTitleMode GetDefaultTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        void* m_Value = nullptr;
        const se::reflect::Class* m_Type = nullptr;
        std::vector<std::shared_ptr<PropertyEditor>> m_Editors;
    };
}
