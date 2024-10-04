#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct WidgetComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(WidgetComponent)

        bool renderingEnabled = true;
    };
}
