#pragma once

#include "engine/reflect/Reflect.h"
#include "engine/input/MouseEvent.h"

namespace se::ui::components
{
    struct MouseInputComponent : reflect::ObjectBase
    {
        SPARK_COMPONENT()

        SPARK_MEMBER()
        std::vector<input::MouseEvent> mouseEvents = {};

        SPARK_MEMBER()
        bool hovered = false;

        SPARK_MEMBER()
        bool lastHovered = false;

        SPARK_MEMBER()
        int buttonMask = 0xFFFFFFFF;

        SPARK_MEMBER()
        int stateMask = static_cast<int>(input::KeyState::Up) | static_cast<int>(input::KeyState::Down);

        SPARK_MEMBER()
        bool receivesScrollEvents = false;
    };
}
