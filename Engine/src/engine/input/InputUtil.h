#pragma once

namespace se::input
{
    struct InputComponent;
    struct KeyEvent;
}

namespace se::input
{
    class InputUtil
    {
    public:
        static void ConsumeKeyEvent(const KeyEvent& keyEvent, InputComponent* input);
    };
}
