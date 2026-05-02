#pragma once

#include "PropertyEditor.h"

namespace se::render
{
    struct UniformValueBase;
}

namespace se::editor::ui::properties
{
    class UniformValueEditor : public PropertyEditor
    {
        SPARK_CLASS()

        public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;
        PropertyTitleMode GetTitleMode() const override;
        ecs::Id GetWidgetId() const override;
        ecs::Id GetTitleId() const override;
        void DestroyUI() override;

    private:
        render::UniformValueBase* m_Value = nullptr;
        const se::reflect::Class* m_Type = nullptr;
        PropertyEditor* m_WrappedEditor = nullptr;
    };
}
