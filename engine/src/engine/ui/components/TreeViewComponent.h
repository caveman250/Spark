#pragma once

#include "spark.h"
#include "engine/ui/Rect.h"
#include "engine/ecs/Component.h"

namespace se::ui::components
{
    struct TreeViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        bool dirty = false;
        Rect lastRect = {};
    };
}
