#pragma once
#include "engine/input/KeyEvent.h"

namespace se::ui::components
{
    struct KeyInputComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER()
        std::vector<input::KeyEvent> keyEvents;

        SPARK_MEMBER()
        input::Key keyMask = static_cast<input::Key>(0xFFFFFFFF);

        SPARK_MEMBER()
        input::KeyState stateMask = static_cast<input::KeyState>(static_cast<int>(input::KeyState::Up) | static_cast<int>(input::KeyState::Down));
    };
}
