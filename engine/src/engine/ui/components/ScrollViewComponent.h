#pragma once

#include "engine/ecs/Signal.h"

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollViewComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<const RectTransformComponent*, float> onScrolled = {};
        float scrollAmount = 0.f;
    };
}
