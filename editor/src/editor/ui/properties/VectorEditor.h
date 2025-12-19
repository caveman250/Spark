#pragma once

#include "PropertyEditor.h"

namespace se::reflect
{
    struct Type_Container;
}

namespace se::editor::ui::properties
{
    class VectorEditor : public PropertyEditor
    {
    SPARK_CLASS()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        PropertyTitleMode GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_VectorType = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
