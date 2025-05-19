#pragma once
#include "engine/input/Key.h"

namespace se::mac
{
    class KeyMap
    {
    public:
        static input::Key MacKeyToSparkKey(uint32_t macKey);
    };
}