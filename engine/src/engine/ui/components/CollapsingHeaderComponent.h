#pragma once
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct CollapsingHeaderComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<bool> onCollapsed = {};
        bool collapsed = true;
        bool lastCollapsed = true;
        size_t fontSize = 16;
    };
}
