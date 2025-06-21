#pragma once

#include "engine/reflect/Reflect_fwd.h"

namespace se
{
    SPARK_ENUM()
    enum class MouseCursor
    {
        Arrow,
        IBeam
    };

    class MouseCursorUtil
    {
    public:
        static void SetMouseCursor(MouseCursor cursor);
    };
}