#import "platform/MouseCursorUtil.h"
#import "Window.h"
#import <AppKit/AppKit.h>
#include "ObjCUtil.h"
#include "wrappers/SparkView.h"
#include "Window.h"
import Application;

namespace se
{
    void MouseCursorUtil::InitCursors()
    {
        // nothing to do.
    }

    void MouseCursorUtil::FreeCursors()
    {
        // nothing to do.
    }

    void MouseCursorUtil::SetMouseCursor(MouseCursor cursor)
    {
        NSCursorPtr selectedCursor = nullptr;
        switch(cursor)
        {
            case MouseCursor::Arrow:
                selectedCursor = platform::mac::SparkView::ArrowCursor();
                break;
            case MouseCursor::IBeam:
                selectedCursor = platform::mac::SparkView::IBeamCursor();
                break;
            case MouseCursor::ResizeLeftRight:
                selectedCursor = platform::mac::SparkView::ResizeLeftRightCursor();
                break;
            case MouseCursor::ResizeUpDown:
                selectedCursor = platform::mac::SparkView::ResizeUpDownCursor();
                break;
        }

        mac::Window* window = static_cast<mac::Window*>(Application::Get()->GetWindow());
        platform::mac::SparkView::SetCursor(window->GetView(), selectedCursor);
    }
}
