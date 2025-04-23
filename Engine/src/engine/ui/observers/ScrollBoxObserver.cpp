
#include "ScrollBoxObserver.h"

#include "engine/Application.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void ScrollBoxObserver::OnAdded(ecs::Id entity, components::ScrollBoxComponent*)
    {
        auto world = Application::Get()->GetWorld();
        if (!world->HasComponent<ui::components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }

//        if (!world->HasComponent<ui::components::MouseInputComponent>(entity))
//        {
//            world->AddComponent<components::MouseInputComponent>(entity);
//        }
    }

    void ScrollBoxObserver::OnRemoved(ecs::Id, components::ScrollBoxComponent*)
    {

    }
}
