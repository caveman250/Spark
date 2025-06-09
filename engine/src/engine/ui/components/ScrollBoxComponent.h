#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct RectTransformComponent;
    struct ScrollBoxComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()
    };
}