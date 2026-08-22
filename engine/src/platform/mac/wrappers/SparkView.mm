#include "SparkView.h"

namespace se::mac::wrappers
{
    void SparkView::SetCursor(SparkViewPtr view, NSCursorPtr cursor)
    {
        [view setCurrentCursor:cursor];
        [cursor set];
    }

    NSCursorPtr SparkView::ArrowCursor()
    {
        return [NSCursor arrowCursor];
    }

    NSCursorPtr SparkView::IBeamCursor()
    {
         return [NSCursor arrowCursor];
    }

     NSCursorPtr SparkView::ResizeLeftRightCursor()
    {
         return [NSCursor resizeLeftRightCursor];
    }

    NSCursorPtr SparkView::ResizeUpDownCursor()
    {
         return [NSCursor resizeUpDownCursor];
    }
}