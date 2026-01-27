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
        static void ProcessMouseEvents(const ecs::Id& entity, InputComponent* input, const std::function<bool(const MouseEvent&)>& func);
        static void ProcessKeyEvents(const ecs::Id& entity, InputComponent* input, const std::function<bool(const KeyEvent&)>& func);

        static bool IsAnyModifierKeyDown(const InputComponent* input);

    private:
        static void ConsumeKeyEvent(InputComponent* input, int index);
        static void ConsumeMouseEvent(InputComponent* input, int index);
    };
}
