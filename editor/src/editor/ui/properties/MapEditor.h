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
        void RecreateElements();
        void CreateElements();
        void DestroyElements();
        void InstantiateElementUI(size_t i);
        void InstantiateElementUI(const std::any& key, void* element);

        void* m_Value = nullptr;
        const reflect::Type_Container* m_MapType = nullptr;
        std::unordered_map<ecs::Id, std::shared_ptr<PropertyEditor>> m_Editors;
        std::unordered_map<ecs::Id, std::string> m_ElementNames;
        ecs::Id m_VerticalBox = ecs::InvalidEntity;
        ecs::Id m_EmptyItem = ecs::InvalidEntity;
        size_t m_CachedMapSize = 0;
    };
}
