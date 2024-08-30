#include "InputUtil.h"

#include "InputComponent.h"

namespace se::input
{
    void InputUtil::ProcessMouseEvents(InputComponent *input, const std::function<bool(const MouseEvent &)> &func)
    {
        if (!input->mouseEvents.empty())
        {
            for (int i = static_cast<int>(input->mouseEvents.size()) - 1; i >= 0; --i)
            {
                const auto& event = input->mouseEvents[i];
                if (func(event))
                {
                    ConsumeMouseEvent(event, input);
                }
            }
        }
    }

    void InputUtil::ProcessKeyEvents(InputComponent *input, const std::function<bool(const KeyEvent &)> &func)
    {
        if (!input->keyEvents.empty())
        {
            for (int i = static_cast<int>(input->keyEvents.size()) - 1; i >= 0; --i)
            {
                const auto& event = input->keyEvents[i];
                if (func(event))
                {
                    ConsumeKeyEvent(event, input);
                }
            }
        }
    }

    void InputUtil::ConsumeKeyEvent(const KeyEvent& keyEvent, InputComponent* input)
    {
        const auto [first, last] = std::ranges::remove(input->keyEvents, keyEvent);
        input->keyEvents.erase(first, last);
    }

    void InputUtil::ConsumeMouseEvent(const MouseEvent &mouseEvent, InputComponent *input)
    {
        const auto [first, last] = std::ranges::remove(input->mouseEvents, mouseEvent);
        input->mouseEvents.erase(first, last);
    }
}
