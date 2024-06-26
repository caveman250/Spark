#include "InputUtil.h"

#include "InputComponent.h"

namespace se::input
{
    void InputUtil::ConsumeKeyEvent(const KeyEvent& keyEvent, InputComponent* input)
    {
        const auto [first, last] = std::ranges::remove(input->keyEvents, keyEvent);
        input->keyEvents.erase(first, last);
    }
}
