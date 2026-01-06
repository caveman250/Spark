#include "platform/MouseCursorUtil.h"
#include "engine/Application.h"
#include "Window.h"

namespace se
{
    void MouseCursorUtil::SetMouseCursor(MouseCursor cursor)
    {
        HCURSOR hCursor = nullptr;
        switch(cursor)
        {
            case MouseCursor::IBeam:
                hCursor = LoadCursor(NULL, IDC_IBEAM);
                break;
            case MouseCursor::ResizeLeftRight:
                hCursor = LoadCursor(NULL, IDC_SIZEWE);
                break;
            case MouseCursor::ResizeUpDown:
                hCursor = LoadCursor(NULL, IDC_SIZENS);
                break;
        }

        auto window = static_cast<windows::Window*>(Application::Get()->GetWindow());
        window->SetCursor(hCursor);
    }
}