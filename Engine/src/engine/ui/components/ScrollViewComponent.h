#pragma once

#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollViewComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT(ScrollViewComponent)

        ecs::Signal<const RectTransformComponent*, float> onScrolled = {};
        float scrollAmount = 0.f;
    };
}
