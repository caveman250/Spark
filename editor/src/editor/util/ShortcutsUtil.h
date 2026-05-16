#pragma once

#include "editor/singleton_components/EditorShortcutsComponent.h"

namespace se::editor::util
{
    template<typename Func, typename Condition>
    void RegisterShortcut(singleton_components::EditorShortcutsComponent* shortcuts,
        const input::Key key,
        const singleton_components::ShortcutModifier modifiers,
        Condition&& condition,
        Func&& func)
    {
        shortcuts->m_Shortcuts[modifiers][key] = { condition, func };
    }
}
