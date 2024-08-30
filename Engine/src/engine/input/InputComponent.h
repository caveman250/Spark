#pragma once

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    struct InputComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_SINGLETON_COMPONENT(InputComponent)

        InputComponent();

        std::vector<KeyEvent> keyEvents;
        std::vector<KeyState::Type> keyStates;

        int lastMouseX = 0;
        int lastMouseY = 0;
        int mouseX = 0;
        int mouseY = 0;
        int mouseDeltaX = 0;
        int mouseDeltaY = 0;
        std::vector<KeyState::Type> mouseButtonStates;
        std::vector<MouseEvent> mouseEvents;
    };
}
