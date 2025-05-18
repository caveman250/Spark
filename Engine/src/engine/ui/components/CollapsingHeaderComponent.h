#pragma once
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct CollapsingHeaderComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(CollapsingHeaderComponent)

        ecs::Signal<bool> onCollapsed = {};
        bool collapsed = true;
        bool lastCollapsed = true;
        size_t fontSize = 16;
    };
}
