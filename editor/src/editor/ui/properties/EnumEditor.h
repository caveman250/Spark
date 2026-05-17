#pragma once

#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class EnumEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;

    private:
        void* m_Value = nullptr;
        size_t m_LastValueIndex = 0;
        ecs::Id m_ComboBox = {};
        ecs::Id m_ComboBoxText = {};
        const reflect::EnumBase* m_EnumType = nullptr;
    };
}
