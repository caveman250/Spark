#pragma once

#include "FloatEditor.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class ClassEditor : public PropertyEditor
    {
    DECLARE_SPARK_CLASS(ClassEditor)

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors) override;
        PropertyTitleMode::Type GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        void* m_Value = nullptr;
        const se::reflect::Class* m_Type = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
