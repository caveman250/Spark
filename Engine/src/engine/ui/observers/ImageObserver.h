#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/ImageComponent.h"

namespace se::ui::observers
{
    class ImageObserver : public ecs::Observer<components::ImageComponent>
    {
        void OnAdded(ecs::Id entity, components::ImageComponent* component) override;
        void OnRemoved(ecs::Id entity, components::ImageComponent* component) override;
    };
}