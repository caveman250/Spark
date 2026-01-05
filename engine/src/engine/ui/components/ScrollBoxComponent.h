#pragma once

#include "engine/ecs/Component.h"

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollBoxComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()
    };
}