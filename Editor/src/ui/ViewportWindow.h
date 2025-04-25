#pragma once

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/ui/Rect.h"

namespace se::editor::ui
{
    class ViewportWindow : public ToolWindow
    {
    public:
        ViewportWindow(EditorRuntime* editor) : ToolWindow(editor) {}
        void Update() override;
        void ConstructUI() override;
        void DestroyUI() override;

        const se::ui::Rect& GetViewportScreenspaceRect() const { return m_ViewportRect; }

    private:
        ecs::Id m_Window;
        ecs::Id m_Viewport;
        se::ui::Rect m_ViewportRect;
    };
}
