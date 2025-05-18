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

        SPARK_MEMBER()
        std::vector<KeyEvent> keyEvents;

        SPARK_MEMBER()
        std::vector<KeyState::Type> keyStates;

        SPARK_MEMBER()
        int lastMouseX = 0;

        SPARK_MEMBER()
        int lastMouseY = 0;

        SPARK_MEMBER()
        int mouseX = 0;

        SPARK_MEMBER()
        int mouseY = 0;

        SPARK_MEMBER()
        int mouseDeltaX = 0;

        SPARK_MEMBER()
        int mouseDeltaY = 0;

        SPARK_MEMBER()
        int mouseScrollDelta = 0;

        SPARK_MEMBER()
        std::vector<KeyState::Type> mouseButtonStates;

        SPARK_MEMBER()
        std::vector<MouseEvent> mouseEvents;
    };
}
