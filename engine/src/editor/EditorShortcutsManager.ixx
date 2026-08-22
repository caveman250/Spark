module;

#include "singleton_components/EditorShortcutsComponent.h"

export module EditorShortcutsManager;

namespace se::editor
{
    export class EditorShortcutsManager
    {
    public:
        static void RegisterShortcuts();

    private:
        static void RegisterGenericShortcuts(singleton_components::EditorShortcutsComponent* shortcuts);

#if SPARK_PLATFORM_MAC
        static void RegisterMacShortcuts(singleton_components::EditorShortcutsComponent* shortcuts);
#endif

#if SPARK_PLATFORM_MAC
        static constexpr auto PrimaryModifier = singleton_components::ShortcutModifier::Super;
#else
        static constexpr auto PrimaryModifier = singleton_components::ShortcutModifier::Ctrl;
#endif
    };
}
