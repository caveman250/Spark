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
    struct RectTransformComponent;
}

namespace se::editor::ui
{
    class OutlineWindow : public ToolWindow
    {
    public:
        OutlineWindow(EditorRuntime* editor) : ToolWindow(editor) {}
        void ConstructUI() override;
        void DestroyUI() override;
        void RebuildOutline();
        void Update() override;

        ecs::Id GetWindow() const { return m_Window; }

    private:
        void AddEntityUI(ecs::World* world, const ecs::Id& entity, const ecs::Id& parent, se::ui::components::RectTransformComponent* treeView);

        ecs::Id m_Window;
        ecs::Id m_TreeViewEntity;
        std::unordered_map<ecs::Id, ecs::Id> m_EntityTexts = {};
        bool m_Valid = false;
    };
}
