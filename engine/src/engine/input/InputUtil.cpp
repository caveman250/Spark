#include "InputUtil.h"
#include "InputComponent.h"

#if SPARK_EDITOR
#include "engine/Application.h"
#include "editor/util/ViewportUtil.h"
#endif

namespace se::input
{
    void InputUtil::ProcessMouseEvents([[maybe_unused]] const ecs::Id& entity,
                                       InputComponent* input,
                                       const std::function<bool(const MouseEvent&)>& func)
    {
#if SPARK_EDITOR
        auto editor = Application::Get()->GetEditor();
        if (entity != ecs::InvalidEntity && *entity.scene != editor->GetEditorScene() &&
            !editor::util::PosWithinViewport(input->mouseX, input->mouseY))
        {
            return;
        }
#endif
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

    void InputUtil::ProcessKeyEvents([[maybe_unused]] const ecs::Id& entity,
                                     InputComponent* input,
                                     const std::function<bool(const KeyEvent&)>& func)
    {
#if SPARK_EDITOR
        auto editor = Application::Get()->GetEditor();
        if (entity != ecs::InvalidEntity &&
            *entity.scene != editor->GetEditorScene() &&
            !editor::util::PosWithinViewport(input->mouseX, input->mouseY))
        {
            return;
        }
#endif

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

    bool InputUtil::IsKeyDown(const InputComponent* input,
        Key key)
    {
        return input->keyStates[static_cast<int>(key)] == KeyState::Down;
    }

    bool InputUtil::IsAnyModifierKeyDown(const InputComponent* input)
    {
        return IsKeyDown(input, Key::LeftControl) ||
               IsKeyDown(input, Key::RightControl) ||
               IsKeyDown(input, Key::LeftAlt) ||
               IsKeyDown(input, Key::RightAlt) ||
               IsKeyDown(input, Key::LeftSuper) ||
               IsKeyDown(input, Key::RightSuper) ||
               IsKeyDown(input, Key::LeftShift) ||
               IsKeyDown(input, Key::RightShift);
    }

    bool InputUtil::IsCtrlDown(const InputComponent* input)
    {
        return IsKeyDown(input, Key::LeftControl) || IsKeyDown(input, Key::RightControl);
    }

    bool InputUtil::IsAltDown(const InputComponent* input)
    {
        return IsKeyDown(input, Key::LeftAlt) || IsKeyDown(input, Key::RightAlt);
    }

    bool InputUtil::IsShiftDown(const InputComponent* input)
    {
        return IsKeyDown(input, Key::LeftShift) || IsKeyDown(input, Key::RightShift);
    }

    bool InputUtil::IsSuperDown(const InputComponent* input)
    {
        return IsKeyDown(input, Key::LeftSuper) || IsKeyDown(input, Key::RightSuper);
    }

    bool InputUtil::IsMouseButtonDown(const InputComponent* input, MouseButton button)
    {
        return input->mouseButtonStates[static_cast<int>(button)] == KeyState::Down;
    }

    void InputUtil::ConsumeKeyEvent(InputComponent* input,
                                    int index)
    {
        input->keyEvents.erase(std::next(input->keyEvents.begin(), index));
    }

    void InputUtil::ConsumeMouseEvent(InputComponent* input,
                                      int index)
    {
        input->mouseEvents.erase(std::next(input->mouseEvents.begin(), index));
    }
}
