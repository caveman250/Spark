#pragma once

#include "engine/ecs/Signal.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::components
{
    struct ScrollViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<const RectTransformComponent*, float> onScrolled = {};
        float scrollAmount = 0.f;
    };
}
