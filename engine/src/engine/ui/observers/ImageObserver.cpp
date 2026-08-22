module;
#include "engine/ui/components/WidgetComponent.h"

module ImageObserver;
import Application;
import ImageComponent;

namespace se::ui::observers
{
    void ImageObserver::OnAdded(const ecs::Id& entity, components::ImageComponent*)
    {
        auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void ImageObserver::OnRemoved(const ecs::Id&, components::ImageComponent*)
    {

    }
}
