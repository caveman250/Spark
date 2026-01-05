#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"

namespace se::ecs::components
{
    struct RootComponent : ecs::Component
    {
        SPARK_COMPONENT()
    };
}
