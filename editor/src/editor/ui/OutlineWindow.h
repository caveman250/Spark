#pragma once

#include "ToolWindow.h"
#include "engine/ui/Rect.h"

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
        void Update() override;

        ecs::Id GetWindow() const { return m_Window; }

    private:
        void AddEntityUI(ecs::World* world, const ecs::Id& entity, const ecs::Id& parent, se::ui::components::RectTransformComponent* treeView) const;

        ecs::Id m_Window;
        ecs::Id m_TreeViewEntity;
        bool m_Valid = false;
    };
}
