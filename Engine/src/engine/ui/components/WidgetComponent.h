#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct WidgetComponent : reflect::ObjectBase
    {
        SPARK_COMPONENT(WidgetComponent)

        bool parentRenderingEnabled = true;
        bool renderingEnabled = true;
        bool updateEnabled = true;
        bool dirty = false;
    };
}
