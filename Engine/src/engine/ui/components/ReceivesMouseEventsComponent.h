#pragma once

#include "engine/reflect/Reflect.h"
#include "engine/input/MouseEvent.h"

namespace se::ui::components
{
    struct ReceivesMouseEventsComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(ReceivesMouseEventsComponent)

        std::vector<input::MouseEvent> mouseEvents = {};
        bool hovered = false;
        bool lastHovered = false;
        int buttonMask = 0xFFFFFFFF;
        int stateMask = input::KeyState::Up | input::KeyState::Down;
    };
}
