#pragma once
#include "engine/input/KeyEvent.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct KeyInputComponent : reflect::ObjectBase
    {
        SPARK_COMPONENT(KeyInputComponent)

        SPARK_MEMBER()
        std::vector<input::KeyEvent> keyEvents;

        SPARK_MEMBER()
        input::Key::Type keyMask = static_cast<input::Key::Type>(0xFFFFFFFF);

        SPARK_MEMBER()
        input::KeyState::Type stateMask = static_cast<input::KeyState::Type>(input::KeyState::Up | input::KeyState::Down);
    };
}
