#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::ecs::components
{
    struct ParentComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ParentComponent)

        int childCount = 0;
    };
}