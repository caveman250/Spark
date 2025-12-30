#pragma once

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/ui/Rect.h"

namespace se::editor::ui
{
    class ViewportWindow : public ToolWindow
    {
    public:
        ViewportWindow(EditorRuntime* editor, const std::function<void(int, int)>& onViewportSizeChanged)
            : ToolWindow(editor)
            , m_OnViewportSizeChanged (onViewportSizeChanged)
        {}
        void Update() override;
        void ConstructUI() override;
        void DestroyUI() override;

        const se::ui::Rect& GetViewportScreenspaceRect() const { return m_ViewportRect; }

        const ecs::Id& GetWindow() const { return m_Window; }

    private:
        bool m_Valid = false;
        ecs::Id m_Window = {};
        ecs::Id m_Viewport = {};
        se::ui::Rect m_ViewportRect = {};
        std::function<void(int, int)> m_OnViewportSizeChanged = nullptr;
    };
}
