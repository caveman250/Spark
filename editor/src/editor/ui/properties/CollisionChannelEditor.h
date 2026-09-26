#pragma once

#include "PropertyEditor.h"
#include "editor/ui/ProjectSettingsWindow.h"
#include "engine/geo/CollisionChannel.h"
#include "engine/math/math.h"

namespace se::editor::ui::properties
{
    class CollisionChannelEditor : public PropertyEditor
    {
        SPARK_CLASS()
    public:
        CollisionChannelEditor() = default;
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        void Update() override;

    private:
        geo::CollisionChannel* m_Value = nullptr;
        geo::CollisionChannel m_LastValue = string::Symbol("");
        ecs::Id m_ComboBox = {};
        ecs::Id m_ComboBoxText = {};
    };
}
