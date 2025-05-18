#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class FontEditor : public PropertyEditor
    {
        SPARK_CLASS(FontEditor)
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors) override;
        void DestroyUI() override;
        PropertyTitleMode::Type GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        asset::Font* m_Value = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
