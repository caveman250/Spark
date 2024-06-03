#pragma once

#include "KeyEvent.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    struct InputComponent
    {
        DECLARE_SPARK_SINGLETON_COMPONENT(InputComponent)

        InputComponent();

        std::vector<KeyEvent> keyEvents;
        std::vector<KeyState::Type> keyStates;
        uint32_t mouseX = 0;
        uint32_t mouseY = 0;
    };
}
