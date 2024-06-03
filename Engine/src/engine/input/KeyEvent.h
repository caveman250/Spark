#pragma once
#include "Key.h"
#include "KeyState.h"

namespace se::input
{
    struct KeyEvent
    {
        DECLARE_SPARK_CLASS(KeyEvent)

        Key::Type key = {};
        KeyState::Type state = {};
    };
}
