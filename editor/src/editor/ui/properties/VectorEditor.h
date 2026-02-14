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
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        PropertyTitleMode GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        std::string CreateEditorName(size_t i, reflect::Type* type) const;
        void InstantiateElementUI(size_t i);
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_VectorType = nullptr;
        std::vector<PropertyEditor*> m_Editors;
        ecs::Id m_VerticalBox = ecs::s_InvalidEntity;
    };
}
