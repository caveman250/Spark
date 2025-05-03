#pragma once

#include "FloatEditor.h"
#include "PropertyEditor.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class SharedPtrEditor : public PropertyEditor
    {
    DECLARE_SPARK_CLASS(SharedPtrEditor)

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const String& name, bool constructTitle) override;
        void Update() override;

    private:
        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_Type = nullptr;
        std::vector<PropertyEditor*> m_Editors;
    };
}
