#pragma once

#include "FloatEditor.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class MapEditor : public PropertyEditor
    {
    DECLARE_SPARK_CLASS(MapEditor)

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors) override;
        PropertyTitleMode::Type GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_VectorType = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
