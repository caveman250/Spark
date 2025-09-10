#import "platform/MouseCursorUtil.h"
#import "engine/Application.h"
#import "Window.h"

namespace se
{
    void MouseCursorUtil::SetMouseCursor(MouseCursor cursor)
    {
        mac::Window* window = static_cast<mac::Window*>(Application::Get()->GetWindow());
        NSCursor* selectedCursor = nullptr;
        switch(cursor)
        {
            case MouseCursor::Arrow:
                selectedCursor = [NSCursor arrowCursor];
                break;
            case MouseCursor::IBeam:
                selectedCursor = [NSCursor IBeamCursor];
                break;
            case MouseCursor::ResizeLeftRight:
                selectedCursor = [NSCursor resizeLeftRightCursor];
                break;
            case MouseCursor::ResizeUpDown:
                selectedCursor = [NSCursor resizeUpDownCursor];
                break;
        }

        [window->GetView() setCurrentCursor:selectedCursor];
        [selectedCursor set];
    }
}