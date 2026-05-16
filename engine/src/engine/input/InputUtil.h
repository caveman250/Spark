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

        static bool IsKeyDown(const InputComponent* input, Key key);
        static bool IsAnyModifierKeyDown(const InputComponent* input);
        static bool IsCtrlDown(const InputComponent* input);
        static bool IsAltDown(const InputComponent* input);
        static bool IsShiftDown(const InputComponent* input);
        static bool IsSuperDown(const InputComponent* input);

        static bool IsMouseButtonDown(const InputComponent* input, MouseButton button);

    private:
        static void ConsumeKeyEvent(InputComponent* input, int index);
        static void ConsumeMouseEvent(InputComponent* input, int index);
    };
}
