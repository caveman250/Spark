#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"

namespace se::asset
{
    class Model;
}

namespace se::editor::ui::properties
{
    class ModelEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        ecs::Id GetWidgetId() const override { return m_WrappedEditor->GetWidgetId(); }
        void Update() override;

    private:
        ecs::Id m_SplitView = {};
        asset::Model* m_Value = nullptr;
        PropertyEditor* m_WrappedEditor = nullptr;
    };
}
