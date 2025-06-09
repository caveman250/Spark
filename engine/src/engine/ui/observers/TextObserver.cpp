
#include "TextObserver.h"

#include "engine/Application.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

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
