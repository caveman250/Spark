#pragma once

#include "KeyState.h"
#include "MouseButton.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    struct MouseEvent : reflect::ObjectBase
    {
        DECLARE_SPARK_CLASS(MouseEvent)

        MouseButton::Type button = {};
        KeyState::Type state = {};

        bool operator==(const MouseEvent& rhs) const
        {
            return button == rhs.button && state == rhs.state;
        }
    };
}
