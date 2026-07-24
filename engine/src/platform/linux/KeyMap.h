#pragma once

#include "engine/input/Key.h"

#if SPARK_PLATFORM_LINUX

#undef linux

namespace se::linux
{
    class KeyMap 
    {
    public:
        static input::Key SDLKeyToSparkKey(SDL_Scancode sdlKey);
    };
}

#endif
