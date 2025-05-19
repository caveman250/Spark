#pragma once
#include "Key.h"
#include "KeyState.h"

namespace se::input
{
    struct KeyEvent : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        Key key = {};

        SPARK_MEMBER(Serialized)
        KeyState state = {};

        bool operator==(const KeyEvent& rhs) const
        {
            return key == rhs.key && state == rhs.state;
        }
    };
}
