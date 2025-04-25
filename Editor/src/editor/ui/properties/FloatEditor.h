#pragma once

#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class FloatEditor : public PropertyEditor
    {
        DECLARE_SPARK_CLASS(FloatEditor)
    public:
        void SetValue(void* value) override;
        void ConstructUI(const String& name, bool constructTitle) override;
        void Update() override;

    private:
        float* m_Value;
        ecs::Id m_Label;
    };
}
