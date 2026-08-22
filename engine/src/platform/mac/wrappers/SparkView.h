#pragma once

#include "engine/render/metal/MTL_fwd.h"

namespace se::mac::wrappers
{
    class SparkView
    {
    public:
        static void SetCursor(SparkViewPtr view, NSCursorPtr cursor);
        static NSCursorPtr ArrowCursor();
        static NSCursorPtr IBeamCursor();
        static NSCursorPtr ResizeLeftRightCursor();
        static NSCursorPtr ResizeUpDownCursor();
    };
}
