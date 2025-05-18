#pragma once
#include "Key.h"
#include "KeyState.h"

namespace se::input
{
    struct KeyEvent : reflect::ObjectBase
    {
        DECLARE_SPARK_CLASS(KeyEvent)

        SPARK_MEMBER(Serialized)
        Key::Type key = {};

        SPARK_MEMBER(Serialized)
        KeyState::Type state = {};

        bool operator==(const KeyEvent& rhs) const
        {
            return key == rhs.key && state == rhs.state;
        }
    };
}
