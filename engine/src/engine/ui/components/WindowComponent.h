#pragma once

#include "spark.h"

namespace se::ui::components
{
    SPARK_ENUM()
    enum class WindowResizeDirection
    {
        Up,
        Down,
        Left,
        Right
    };

    struct WindowComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        SPARK_MEMBER()
        float pendingDeltaX = 0;

        SPARK_MEMBER()
        float pendingDeltaY = 0;

        SPARK_MEMBER()
        bool isShowingResizeCursor = false;

        SPARK_MEMBER()
        bool isResizing = false;

        SPARK_MEMBER()
        WindowResizeDirection resizeDirection = WindowResizeDirection::Up;
    };
}
