#include "EditorShortcutsManager.h"

#include "engine/input/InputUtil.h"
#include "engine/input/KeyEvent.h"

namespace se::editor
{
    void EditorShortcutsManager::ProcessInput(input::InputComponent* input)
    {
        auto modifierMask = ShortcutModifier::None;
        if (input::InputUtil::IsCtrlDown(input))
        {
            modifierMask |= ShortcutModifier::Ctrl;
        }
        if (input::InputUtil::IsAltDown(input))
        {
            modifierMask |= ShortcutModifier::Alt;
        }
        if (input::InputUtil::IsShiftDown(input))
        {
            modifierMask |= ShortcutModifier::Shift;
        }
        if (input::InputUtil::IsSuperDown(input))
        {
            modifierMask |= ShortcutModifier::Super;
        }

        const auto& shortcuts = m_Shortcuts[modifierMask];

        input::InputUtil::ProcessKeyEvents(ecs::InvalidEntity, input, [shortcuts](const input::KeyEvent& ev)
        {
            if (ev.state == input::KeyState::Down)
            {
                auto it = shortcuts.find(ev.key);
                if (it != shortcuts.end())
                {
                    it->second();
                    return true;
                }
            }
            return false;
        });
    }
}
