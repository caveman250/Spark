#pragma once

#include "engine/input/Key.h"

#undef linux

namespace se::linux
{
    class KeyMap 
    {
    public:
        static input::Key SDLKeyToSparkKey(SDL_Scancode sdlKey);
    };
}
