#pragma once

#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class MapEditor : public PropertyEditor
    {
    SPARK_CLASS()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const PropertyEditorParams& params) override;
        PropertyTitleMode GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_VectorType = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
