#pragma once

#include <engine/ecs/Signal.h>

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollBoxComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ScrollBoxComponent)
    };
}
