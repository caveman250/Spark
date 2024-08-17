#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ecs::components
{
    struct RootComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(RootComponent)
    };
}
