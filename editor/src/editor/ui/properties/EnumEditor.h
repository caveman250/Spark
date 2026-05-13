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
        ecs::Id m_ComboBox = {};
        const reflect::EnumBase* m_EnumType = nullptr;
    };
}
