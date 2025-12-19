#include "platform/MouseCursorUtil.h"
#include "engine/Application.h"
#include "Window.h"

namespace se
{
    void MouseCursorUtil::SetMouseCursor(MouseCursor cursor)
    {
        //mac::Window* window = static_cast<mac::Window*>(Application::Get()->GetWindow());
        //NSCursor* selectedCursor = nullptr;
        //switch(cursor)
        //{
        //    case MouseCursor::Arrow:
        //        selectedCursor = [NSCursor arrowCursor];
        //    break;
        //    case MouseCursor::IBeam:
        //        selectedCursor = [NSCursor IBeamCursor];
        //    break;
        //}
//
        //[window->GetView() setCurrentCursor:selectedCursor];
        //[selectedCursor set];
    }
}