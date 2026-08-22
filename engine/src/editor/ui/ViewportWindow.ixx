module;

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"

export module ViewportWindow;
import EditorMode;
import Spark.UI.Rect;
namespace se::editor::ui
{
    export class ViewportWindow : public ToolWindow
    {
    public:
        ViewportWindow(Editor* editor, const std::function<void(int, int)>& onViewportSizeChanged)
            : ToolWindow(editor)
            , m_OnViewportSizeChanged (onViewportSizeChanged)
        {}
        void Update() override;
        void ConstructUI() override;
        void DestroyUI() override;

        const se::ui::Rect& GetViewportScreenspaceRect() const { return m_ViewportRect; }
        const ecs::Id& GetWindow() const { return m_Window; }


    private:
        void HandleDragDrop();
        void UpdatePlayButtonTexture() const;
        void UpdatePauseButtonTexture() const;

        bool m_Valid = false;
        bool m_ShouldToggleGameMode = false;
        bool m_UIVisible = true;
        ecs::Id m_Window = {};
        ecs::Id m_Viewport = {};
        ecs::Id m_PlayButton = {};
        ecs::Id m_PauseButton = {};
        ecs::Id m_UIVisibleCheckBox = {};
        se::ui::Rect m_ViewportRect = {};
        std::function<void(int, int)> m_OnViewportSizeChanged = nullptr;
        EditorMode m_LastMode = EditorMode::Default;
    };
}
