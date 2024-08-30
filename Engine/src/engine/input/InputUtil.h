#pragma once
#include "MouseEvent.h"

namespace se::input
{
    struct InputComponent;
    struct KeyEvent;
    struct MouseEvent;
}

namespace se::input
{
    class InputUtil
    {
    public:
        static void ProcessMouseEvents(InputComponent* input, const std::function<bool(const MouseEvent&)>& func);
        static void ProcessKeyEvents(InputComponent* input, const std::function<bool(const KeyEvent&)>& func);

    private:
        static void ConsumeKeyEvent(const KeyEvent& keyEvent, InputComponent* input);
        static void ConsumeMouseEvent(const MouseEvent& mouseEvent, InputComponent* input);
    };
}
