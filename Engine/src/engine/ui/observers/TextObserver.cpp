
#include "TextObserver.h"

#include "engine/Application.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void TextObserver::OnAdded(ecs::Id entity, components::TextComponent*)
    {
        auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<ui::components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void TextObserver::OnRemoved(ecs::Id, components::TextComponent*)
    {

    }
}
