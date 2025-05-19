#pragma once

#include "KeyState.h"
#include "MouseButton.h"
#include "engine/math/IntVec2.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    struct MouseEvent : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER()
        MouseButton::Type button = {};

        SPARK_MEMBER()
        KeyState::Type state = KeyState::Down;

        SPARK_MEMBER()
        int scrollDelta = 0;

        bool operator==(const MouseEvent& rhs) const
        {
            return button == rhs.button && state == rhs.state && scrollDelta == rhs.scrollDelta;
        }
    };
}
