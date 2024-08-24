#pragma once

#include "spark.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct ButtonComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ButtonComponent)
    };
}
