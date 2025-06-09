#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class FontEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        void DestroyUI() override;
        PropertyTitleMode GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        asset::Font* m_Value = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
