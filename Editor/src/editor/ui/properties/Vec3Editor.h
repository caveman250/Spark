#pragma once

#include "FloatEditor.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class Vec3Editor : public PropertyEditor
    {
        DECLARE_SPARK_CLASS(Vec3Editor)
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors) override;
        PropertyTitleMode::Type GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        math::Vec3* m_Value;
        FloatEditor m_XEditor;
        FloatEditor m_YEditor;
        FloatEditor m_ZEditor;
    };
}
