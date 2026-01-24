#pragma once

#include "engine/ecs/Component.h"
#include "engine/input/MouseEvent.h"

namespace se::ui::components
{
    struct MouseInputComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER()
        bool enabled = true;

        SPARK_MEMBER()
        std::vector<input::MouseEvent> mouseEvents = {};

        SPARK_MEMBER()
        bool hovered = false;

        SPARK_MEMBER()
        bool lastHovered = false;

        SPARK_MEMBER(Serialized)
        int buttonMask = 0xFFFFFFFF;

        SPARK_MEMBER(Serialized)
        int stateMask = static_cast<int>(input::KeyState::Up) | static_cast<int>(input::KeyState::Down);

        SPARK_MEMBER(Serialized)
        bool receivesScrollEvents = false;

        SPARK_MEMBER()
        bool isMouseDown = false;
    };
}
