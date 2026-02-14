#pragma once

#include "NumberEditor.h"
#include "PropertyEditor.h"


namespace se::editor::ui::properties
{
    class SceneEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        PropertyTitleMode GetTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        ecs::Id m_SplitView = {};
        ecs::SceneSaveData* m_Value = nullptr;
    };
}
