#include "EditorShortcutsSystem.h"

#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"
#include "engine/ui/systems/EditableTextSystem.h"
#include "engine/ui/systems/UIKeyboardInputSystem.h"

namespace se::editor::systems
{
    ecs::SystemDeclaration EditorShortcutsSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("Editor Shortcuts System")
            .WithSingletonComponent<singleton_components::EditorShortcutsComponent>()
            .WithSingletonComponent<input::InputComponent>()
            .WithDependency<se::ui::systems::UIKeyboardInputSystem>();
    }

    void EditorShortcutsSystem::OnUpdate(const ecs::QueryResults& results)
    {
        auto editor = Application::Get()->GetEditorRuntime();
        if (editor->InGameMode())
        {
            return;
        }

        auto* shortcutsComp = results.updateData.GetSingletonComponent<singleton_components::EditorShortcutsComponent>();
        auto* inputComp = results.updateData.GetSingletonComponent<input::InputComponent>();

        shortcutsComp->pendingShortcuts.clear();

        auto modifierMask = singleton_components::ShortcutModifier::None;
        if (input::InputUtil::IsCtrlDown(inputComp))
        {
            modifierMask |= singleton_components::ShortcutModifier::Ctrl;
        }
        if (input::InputUtil::IsAltDown(inputComp))
        {
            modifierMask |= singleton_components::ShortcutModifier::Alt;
        }
        if (input::InputUtil::IsShiftDown(inputComp))
        {
            modifierMask |= singleton_components::ShortcutModifier::Shift;
        }
        if (input::InputUtil::IsSuperDown(inputComp))
        {
            modifierMask |= singleton_components::ShortcutModifier::Super;
        }

        const auto& shortcuts = shortcutsComp->m_Shortcuts[modifierMask];

        input::InputUtil::ProcessKeyEvents(ecs::InvalidEntity, inputComp, [shortcuts, shortcutsComp](const input::KeyEvent& ev)
        {
            if (ev.state == input::KeyState::Down)
            {
                auto it = shortcuts.find(ev.key);
                if (it != shortcuts.end())
                {
                    if (it->second.condition())
                    {
                        shortcutsComp->pendingShortcuts.push_back(it->second.callback);
                        return true;
                    }
                }
            }
            return false;
        });
    }
}
