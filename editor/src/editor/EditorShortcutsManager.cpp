#include "EditorShortcutsManager.h"
#include "engine/Application.h"
#include "platform/PlatformRunLoop.h"
#include "util/ShortcutsUtil.h"
#include "Transactions.h"

#if SPARK_PLATFORM_MAC
#include "platform/mac/MacRunLoop.h"
#endif

namespace se
{
    void editor::EditorShortcutsManager::RegisterShortcuts()
    {
        auto* app = Application::Get();
        auto* world = app->GetWorld();

        auto* shortcutsComp = world->GetSingletonComponent<singleton_components::EditorShortcutsComponent>();
        RegisterGenericShortcuts(shortcutsComp);
#if SPARK_PLATFORM_MAC
        RegisterMacShortcuts(shortcutsComp);
#endif
    }

    void editor::EditorShortcutsManager::RegisterGenericShortcuts(singleton_components::EditorShortcutsComponent* shortcuts)
    {
        auto* editor = Application::Get()->GetEditor();

        util::RegisterShortcut(shortcuts, input::Key::S, PrimaryModifier,
                               []()
                               {
                                   return true;
                               },
                               [editor]()
                               {
                                   editor->SaveAll();
                               });

        util::RegisterShortcut(shortcuts, input::Key::W, singleton_components::ShortcutModifier::None,
                               [editor]()
                               {
                                   return editor->GetSelectedEntity() != ecs::InvalidEntity && editor->GetGizmoManager().GetGizmoType() != GizmoType::Translate;
                               },
                               [editor]()
                               {
                                   editor->GetGizmoManager().SetGizmoType(GizmoType::Translate);
                               });

        util::RegisterShortcut(shortcuts, input::Key::E, singleton_components::ShortcutModifier::None,
                               [editor]()
                               {
                                   return editor->GetSelectedEntity() != ecs::InvalidEntity && editor->GetGizmoManager().GetGizmoType() != GizmoType::Rotate;
                               },
                               [editor]()
                               {
                                   editor->GetGizmoManager().SetGizmoType(GizmoType::Rotate);
                               });

        util::RegisterShortcut(shortcuts, input::Key::Escape, singleton_components::ShortcutModifier::None,
                               []()
                               {
                                   return true;
                               },
                               [editor]()
                               {
                                   editor->DeSelectAll();
                               });

        util::RegisterShortcut(shortcuts, input::Key::C, PrimaryModifier,
                       [editor]()
                       {
                           return editor->HasValidCopySelection();
                       },
                       [editor]()
                       {
                           editor->Copy();
                       });

        util::RegisterShortcut(shortcuts, input::Key::X, PrimaryModifier,
                      [editor]()
                      {
                          return editor->HasValidCopySelection();
                      },
                      [editor]()
                      {
                          editor->Cut();
                      });

        // Paste
        util::RegisterShortcut(shortcuts, input::Key::V, PrimaryModifier,
               [editor]()
               {
                   return editor->HasValidPasteTarget();
               },
               [editor]()
               {
                   editor->Paste();
               });

        // Undo
        util::RegisterShortcut(shortcuts, input::Key::Z, PrimaryModifier,
           []()
           {
               return true;
           },
           []()
           {
               Transactions::Get()->Undo();
           });

        // Redo
        util::RegisterShortcut(shortcuts, input::Key::Y, PrimaryModifier,
           []()
           {
               return true;
           },
           []()
           {
               Transactions::Get()->Redo();
           });
    }

#if SPARK_PLATFORM_MAC
    void editor::EditorShortcutsManager::RegisterMacShortcuts(singleton_components::EditorShortcutsComponent* shortcuts)
    {
        util::RegisterShortcut(shortcuts, input::Key::Q, singleton_components::ShortcutModifier::Super,
                               []()
                               {
                                   return true;
                               },
                               []()
                               {
                                   auto* runLoop = PlatformRunLoop::Get();
                                   runLoop->RequestExit();
                               });
    }
#endif
}
