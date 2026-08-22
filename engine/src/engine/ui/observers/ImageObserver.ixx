module;
#include "engine/ecs/Observer.h"

export module ImageObserver;
import ImageComponent;

namespace se::ui::observers
{
    export class ImageObserver : public ecs::Observer<components::ImageComponent>
    {
        void OnAdded(const ecs::Id& entity, components::ImageComponent* component) override;
        void OnRemoved(const ecs::Id& entity, components::ImageComponent* component) override;
    };
}
