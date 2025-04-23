
#include "EditableTextObserver.h"

#include "engine/Application.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void EditableTextObserver::OnAdded(ecs::Id entity, components::EditableTextComponent*)
    {
        auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<ui::components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }

        if (!world->HasComponent<ui::components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }

        if (!world->HasComponent<ui::components::KeyInputComponent>(entity))
        {
            world->AddComponent<components::KeyInputComponent>(entity);
        }
    }

    void EditableTextObserver::OnRemoved(ecs::Id, components::EditableTextComponent*)
    {

    }
}
