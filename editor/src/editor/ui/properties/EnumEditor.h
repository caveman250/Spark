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
        void ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground) override;
        void Update() override;

    private:
        int* m_Value = nullptr;
        int m_LastValue = 0;
        ecs::Id m_ComboBox = {};
        const reflect::Enum* m_EnumType = nullptr;
    };
}
