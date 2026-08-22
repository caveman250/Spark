module;
#include "engine/ui/components/WidgetComponent.h"

module TextObserver;

import Application;
import TextComponent;

namespace se::ui::observers
{
    void TextObserver::OnAdded(const ecs::Id& entity, components::TextComponent*)
    {
        auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void TextObserver::OnRemoved(const ecs::Id&, components::TextComponent*)
    {

    }
}
