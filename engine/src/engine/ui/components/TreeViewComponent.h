#pragma once

#include "spark.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct TreeViewComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        bool dirty = false;
        Rect lastRect = {};
    };
}
