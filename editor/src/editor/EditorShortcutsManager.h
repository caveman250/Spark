#pragma once
#include "engine/bits/EnumClassBitwise.h"
#include "engine/input/Key.h"

namespace se::input
{
    struct InputComponent;
}

namespace se::editor
{
    enum class ShortcutModifier
    {
        None = 0,
        Ctrl = 1 << 0,
        Alt = 1 << 1,
        Shift = 1 << 2,
        Super = 1 << 3,
    };
    ENUM_CLASS_ENABLE_BITWISE(ShortcutModifier)

    class EditorShortcutsManager
    {
    public:
        template <typename Func>
        void RegisterShortcut(input::Key key, ShortcutModifier modifiers, Func&& func);

        void ProcessInput(input::InputComponent* input);

    private:
        typedef std::unordered_map<input::Key, std::function<void()>> Shortcuts;
        std::unordered_map<ShortcutModifier, Shortcuts> m_Shortcuts;
    };

    template<typename Func>
    void EditorShortcutsManager::RegisterShortcut(input::Key key,
        const ShortcutModifier modifiers,
        Func&& func)
    {
        m_Shortcuts[modifiers][key] = func;
    }
}
