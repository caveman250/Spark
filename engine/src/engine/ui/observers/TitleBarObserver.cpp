
module;



#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

module TitleBarObserver;
import Application;
import TitleBarComponent;
import ImageComponent;
import MaterialInstance;

namespace se::ui::observers
{
    void TitleBarObserver::OnAdded(const ecs::Id& entity, components::TitleBarComponent*)
    {
        const auto world = Application::Get()->GetWorld();

        //background
        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            const auto image = world->AddComponent<components::ImageComponent>(entity);
            image->materialInstance = std::make_shared<render::MaterialInstance>("/engine_assets/materials/editor_darkbg.sass");
        }

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }

        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }
    }

    void TitleBarObserver::OnRemoved(const ecs::Id&, components::TitleBarComponent*)
    {

    }
}
