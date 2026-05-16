#include "EditorShortcutsManager.h"
#include "engine/Application.h"
#include "util/ShortcutsUtil.h"

namespace se
{
    void editor::EditorShortcutsManager::RegisterShortcuts()
    {
#if SPARK_PLATFORM_MAC
        constexpr auto primaryModifier = singleton_components::ShortcutModifier::Super;
#else
        ShortcutModifier primaryModifier = singleton_components::ShortcutModifier::Ctrl;
#endif

        auto* app = Application::Get();
        auto* world = app->GetWorld();
        auto* editor = app->GetEditor();

        auto* shortcutsComp = world->GetSingletonComponent<singleton_components::EditorShortcutsComponent>();
        util::RegisterShortcut(shortcutsComp, input::Key::S, primaryModifier,
                               []()
                               {
                                   return true;
                               },
                               [editor]()
                               {
                                   editor->SaveAll();
                               });

        util::RegisterShortcut(shortcutsComp, input::Key::W, singleton_components::ShortcutModifier::None,
                               [editor]()
                               {
                                   return editor->GetSelectedEntity() != ecs::InvalidEntity && editor->GetGizmoManager().GetGizmoType() != GizmoType::Translate;
                               },
                               [editor]()
                               {
                                   editor->GetGizmoManager().SetGizmoType(GizmoType::Translate);
                               });

        util::RegisterShortcut(shortcutsComp, input::Key::E, singleton_components::ShortcutModifier::None,
                               [editor]()
                               {
                                   return editor->GetSelectedEntity() != ecs::InvalidEntity && editor->GetGizmoManager().GetGizmoType() != GizmoType::Rotate;
                               },
                               [editor]()
                               {
                                   editor->GetGizmoManager().SetGizmoType(GizmoType::Rotate);
                               });

        util::RegisterShortcut(shortcutsComp, input::Key::Escape, singleton_components::ShortcutModifier::None,
                               []()
                               {
                                   return true;
                               },
                               [editor]()
                               {
                                   editor->DeSelectAll();
                               });
    }
}
