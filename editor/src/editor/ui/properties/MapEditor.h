#pragma once

#include "PropertyEditor.h"

namespace se::editor::ui::properties
{
    class MapEditor : public PropertyEditor
    {
    SPARK_CLASS()

    public:
        void SetValue(void* value, const reflect::Type* type) override;
        void* GetValue() const override { return m_Value; }
        void ConstructUI(const PropertyEditorParams& params) override;
        PropertyTitleMode GetDefaultTitleMode() const override { return PropertyTitleMode::NextLine; }
        void Update() override;

    private:
        void InstantiateElementUI(size_t i);
        void InstantiateElementUI(const std::any& key, void* element);

        void* m_Value = nullptr;
        const se::reflect::Type_Container* m_MapType = nullptr;
        std::unordered_map<ecs::Id, std::shared_ptr<PropertyEditor>> m_Editors;
        std::unordered_map<ecs::Id, std::string> m_ElementNames;
        ecs::Id m_VerticalBox = ecs::InvalidEntity;
    };
}
