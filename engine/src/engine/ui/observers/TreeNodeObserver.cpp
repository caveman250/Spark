module;

#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

module TreeNodeObserver;

import Application;
import TreeNodeComponent;
import TextComponent;

namespace se::ui::observers
{
    void TreeNodeObserver::OnAdded(const ecs::Id& entity, components::TreeNodeComponent*)
    {
        auto world = Application::Get()->GetWorld();
        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void TreeNodeObserver::OnRemoved(const ecs::Id&, components::TreeNodeComponent*)
    {

    }
}
