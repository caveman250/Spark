#pragma once
#include "engine/input/Key.h"

namespace se::windows
{
    class KeyMap
    {
    public:
        static input::Key::Type WindowsKeyToSparkKey(uint32_t winKey);
    };
}
