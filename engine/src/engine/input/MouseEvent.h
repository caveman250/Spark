#pragma once

#include "KeyState.h"
#include "MouseButton.h"
#include "engine/reflect/Object.h"

namespace se::input
{
    struct MouseEvent : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER()
        MouseButton button = {};

        SPARK_MEMBER()
        KeyState state = KeyState::Down;

        SPARK_MEMBER()
        int scrollDelta = 0;

        bool operator==(const MouseEvent& rhs) const
        {
            return button == rhs.button && state == rhs.state && scrollDelta == rhs.scrollDelta;
        }
    };
}
