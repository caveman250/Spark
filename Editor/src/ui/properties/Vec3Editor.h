#pragma once

#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class Vec3Editor : public PropertyEditor
    {
        DECLARE_SPARK_CLASS(Vec3Editor)
    public:
        void SetValue(void* value) override;
        void ConstructUI(const String& name, bool constructTitle) override;
        void Update() override;

    private:
        math::Vec3* m_Value;
        FloatEditor m_XEditor;
        FloatEditor m_YEditor;
        FloatEditor m_ZEditor;
    };
}
