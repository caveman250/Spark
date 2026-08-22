module;



#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

module ButtonObserver;
import Application;
import ButtonComponent;
import Material;
import MaterialInstance;
import ImageComponent;

namespace se::ui::observers
{
    void ButtonObserver::OnAdded(const ecs::Id& entity, components::ButtonComponent *component)
    {
        const auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto* image = world->AddComponent<components::ImageComponent>(entity);

            image->materialInstance = std::make_shared<render::MaterialInstance>("/engine_assets/materials/ui_alpha_texture.sass");
            image->materialInstance->SetUniform("Texture", 1, &component->image);
        }

        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }

        if (!world->HasComponent<components::RectTransformComponent>(entity))
        {
            world->AddComponent<components::RectTransformComponent>(entity);
        }
    }

    void ButtonObserver::OnRemoved(const ecs::Id&, components::ButtonComponent*)
    {
        // leave image there for time being. see how annoying it is in practice.
    }
}
