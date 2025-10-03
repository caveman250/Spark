#include "ViewportUtil.h"

#include "editor/EditorRuntime.h"
#include "engine/Application.h"
#include "engine/math/math.h"
#include "engine/render/Renderer.h"
#include "platform/IWindow.h"

namespace se::editor::util
{
    math::IntVec2 ScreenSpaceToGameViewportSpace(int mouseX,
                                                 int mouseY)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();

        se::ui::Rect viewportRect = editor->GetViewportRect();
        math::IntVec2 renderTargetSize = editor->GetFrameBuffer()->GetSize();
        math::Vec2 relative = math::Vec2(static_cast<float>(mouseX - viewportRect.topLeft.x) / viewportRect.size.x,
                                         static_cast<float>(mouseY - viewportRect.topLeft.y) / viewportRect.size.y);

        return relative * math::Vec2(renderTargetSize) * app->GetWindow()->GetContentScale();
    }

    bool PosWithinViewport(int mouseX,
                           int mouseY)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        se::ui::Rect viewportRect = editor->GetViewportRect();
        return mouseX <= viewportRect.topLeft.x + viewportRect.size.x &&
               mouseY <= viewportRect.topLeft.y + viewportRect.size.y &&
               mouseX >= viewportRect.topLeft.x &&
               mouseY >= viewportRect.topLeft.y;
    }
}
