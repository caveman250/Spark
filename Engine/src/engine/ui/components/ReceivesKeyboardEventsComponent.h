#pragma once
#include "engine/input/KeyEvent.h"
#include "engine/reflect/Reflect.h"

namespace se::ui::components
{
    struct ReceivesKeyboardEventsComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ReceivesKeyboardEventsComponent)

        std::vector<input::KeyEvent> keyEvents;
        input::Key::Type keyMask = static_cast<input::Key::Type>(0xFFFFFFFF);
        input::KeyState::Type stateMask = static_cast<input::KeyState::Type>(input::KeyState::Up | input::KeyState::Down);
    };
}
