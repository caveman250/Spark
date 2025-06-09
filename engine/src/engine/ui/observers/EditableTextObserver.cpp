
#include "EditableTextObserver.h"

#include "engine/Application.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

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
            world->AddComponent<components::KeyInputComponent>(entity);
        }
    }

    void EditableTextObserver::OnRemoved(const ecs::Id&, components::EditableTextComponent*)
    {

    }
}
