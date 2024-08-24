#pragma once

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
        static void ConsumeKeyEvent(const KeyEvent& keyEvent, InputComponent* input);
        static void ConsumeMouseEvent(const MouseEvent& mouseEvent, InputComponent* input);
    };
}
