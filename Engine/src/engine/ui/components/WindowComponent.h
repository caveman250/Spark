#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct WindowComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(WindowComponent)
    };
}
