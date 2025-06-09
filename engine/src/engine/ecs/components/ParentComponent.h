#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs::components
{
    struct ParentComponent : reflect::ObjectBase
    {
        SPARK_COMPONENT()
    };
}
