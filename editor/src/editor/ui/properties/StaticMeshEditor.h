#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class StaticMeshEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const PropertyEditorParams& params) override;
        PropertyTitleMode GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        asset::StaticMesh* m_Value = nullptr;
    };
}
