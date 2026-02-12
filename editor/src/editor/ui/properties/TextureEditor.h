#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class TextureEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void ConstructUI(const PropertyEditorParams& params) override;
        ecs::Id GetWidgetId() const override { return m_WrappedEditor->GetWidgetId(); }
        void Update() override;

    private:
        ecs::Id m_SplitView = {};
        asset::Texture* m_Value = nullptr;
        PropertyEditor* m_WrappedEditor = nullptr;
    };
}
