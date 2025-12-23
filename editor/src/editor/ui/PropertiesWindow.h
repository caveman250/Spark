#pragma once
#include "ToolWindow.h"
#include "properties/PropertyEditor.h"

namespace se::asset
{
    class Asset;
    class Font;
}

namespace se::ecs
{
    class World;
}

namespace se::ui::components
{
    struct VerticalBoxComponent;
}

namespace se::editor::ui
{
    class PropertiesWindow : public ToolWindow
    {
    public:
        PropertiesWindow(EditorRuntime* editor) : ToolWindow(editor) {}
        void Update() override;
        void ConstructUI() override;
        void DestroyUI() override;

        void RebuildProperties();

        const ecs::Id& GetWindow() const { return m_Window; }
    private:
        void AddEntityProperties(const ecs::Id& entity,
                                 ecs::World* world,
                                 const std::shared_ptr<asset::Font>& font);
        void AddSingletonComponentProperties(reflect::ObjectBase* selectedSingletonComp);
        void AddAssetProperties(const std::shared_ptr<asset::Asset>& asset,
                                ecs::World* world,
                                const std::shared_ptr<asset::Font>& font);

        ecs::Id m_Window;
        ecs::Id m_ScrollBoxContent;
        bool m_Valid = false;
        std::vector<properties::PropertyEditor*> m_PropertyEditors = {};
    };
}
