#pragma once
#include "engine/input/Key.h"

namespace se::mac
{
    class KeyMap
    {
    public:
        static input::Key::Type MacKeyToSparkKey(uint32_t macKey);
    };
}