#pragma once

#include "spark.h"

namespace se::ui::components
{
    struct WindowComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        SPARK_MEMBER()
        float pendingDeltaX = 0;

        SPARK_MEMBER()
        float pendingDeltaY = 0;
    };
}
