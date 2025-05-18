#pragma once

#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct TextCaretComponent : reflect::ObjectBase
    {
        SPARK_COMPONENT(TextCaretComponent)

        bool active = false;
        float currentStateTime = 0.f;
    };
}
