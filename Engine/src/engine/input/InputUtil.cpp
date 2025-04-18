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
                SPARK_ASSERT(!input->mouseEvents.empty());
                const auto& event = input->mouseEvents[i];
                if (func(event))
                {
                    ConsumeMouseEvent(input, i);
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
                    ConsumeKeyEvent(input, i);
                }
            }
        }
    }

    void InputUtil::ConsumeKeyEvent(InputComponent* input, int index)
    {
        input->keyEvents.erase(std::next(input->keyEvents.begin(), index));
    }

    void InputUtil::ConsumeMouseEvent(InputComponent *input, int index)
    {
        input->mouseEvents.erase(std::next(input->mouseEvents.begin(), index));
    }
}
