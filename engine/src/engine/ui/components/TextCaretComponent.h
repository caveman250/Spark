#pragma once

#include "engine/ecs/Component.h"

namespace se::ui::components
{
    struct TextCaretComponent : ecs::Component
    {
        SPARK_COMPONENT()

        bool active = false;
        float currentStateTime = 0.f;
    };
}
