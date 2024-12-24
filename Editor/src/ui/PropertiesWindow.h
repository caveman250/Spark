#pragma once
#include "ToolWindow.h"
#include "properties/PropertyEditor.h"

namespace se::ecs
{
    class World;
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
    private:
        ecs::Id m_Window;
        ecs::Id m_ScrollBoxContent;
        bool m_Valid = false;
        std::vector<properties::PropertyEditor*> m_PropertyEditors = {};
    };
}
