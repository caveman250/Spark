module;
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

module EditableTextObserver;
import Application;
import EditableTextComponent;

namespace se::ui::observers
{
    void EditableTextObserver::OnAdded(const ecs::Id& entity, components::EditableTextComponent*)
    {
        auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }

        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }

        if (!world->HasComponent<components::KeyInputComponent>(entity))
        {
            auto* keyInput = world->AddComponent<components::KeyInputComponent>(entity);
            keyInput->keyMask = input::Key::Unknown;
        }
    }

    void EditableTextObserver::OnRemoved(const ecs::Id&, components::EditableTextComponent*)
    {

    }
}
