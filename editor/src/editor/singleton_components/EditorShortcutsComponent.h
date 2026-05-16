#pragma once
#include "engine/bits/EnumClassBitwise.h"
#include "engine/ecs/Component.h"
#include "engine/input/Key.h"

namespace se::input
{
    struct InputComponent;
}

namespace se::editor::singleton_components
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

    struct EditorShortcut
    {
        std::function<bool()> condition;
        std::function<void()> callback;
    };

    class EditorShortcutsComponent : public ecs::Component
    {
        SPARK_SINGLETON_COMPONENT(EditorOnly)

        typedef std::unordered_map<input::Key, EditorShortcut> Shortcuts;
        std::unordered_map<ShortcutModifier, Shortcuts> shortcuts;
        std::vector<std::function<void()>> pendingShortcuts = {};
    };
}
