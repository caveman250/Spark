#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct WindowComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT(WindowComponent)
    };
}
