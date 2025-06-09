#include "ViewportUtil.h"

#include "engine/Application.h"
#include "platform/IWindow.h"

namespace se::editor::util
{
    math::IntVec2 ScreenSpaceToGameViewportSpace(int mouseX,
                                                 int mouseY)
    {
        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        se::ui::Rect viewportRect = editor->GetViewportRect();
        math::Vec2 relative = math::Vec2((float)(mouseX - viewportRect.topLeft.x) / viewportRect.size.x,
                                         (float)(mouseY - viewportRect.topLeft.y) / viewportRect.size.y);

        auto window = app->GetPrimaryWindow();
        return relative * math::Vec2(static_cast<float>(window->GetWidth()),
                                     static_cast<float>(window->GetHeight()));
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
