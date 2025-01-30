#pragma once
#include "ToolWindow.h"

namespace se::ecs
{
    class World;
}
namespace se::ui::components
{
    struct TreeViewComponent;
}

namespace se::editor::ui
{
    class OutlineWindow : public ToolWindow
    {
    public:
        OutlineWindow(EditorRuntime* editor) : ToolWindow(editor) {}
        void ConstructUI() override;
        void DestroyUI() override;
        void RebuildOutline() const;

    private:
        void AddEntityUI(ecs::World* world, ecs::Id entity, ecs::Id parent, se::ui::components::TreeViewComponent* treeView) const;

        ecs::Id m_Window;
        ecs::Id m_TreeViewEntity;
        bool m_Valid = false;
    };
}
