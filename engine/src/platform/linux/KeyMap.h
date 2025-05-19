#pragma once

#include "spark.h"
#include "engine/input/Key.h"

#undef linux

namespace se::linux
{
    class KeyMap 
    {
    public:
        static input::Key::Type SDLKeyToSparkKey(SDL_Scancode sdlKey);
    };
}
