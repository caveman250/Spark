#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class Vec3Editor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;

    private:
        math::Vec3* m_Value;
        NumberEditor<float> m_XEditor;
        NumberEditor<float> m_YEditor;
        NumberEditor<float> m_ZEditor;
    };
}
