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

        int mouseX = 0;
        int mouseY = 0;
        std::vector<KeyState::Type> mouseButtonStates;
    };
}
