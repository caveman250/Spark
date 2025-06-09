#pragma once

#include "engine/ecs/Observer.h"
#include "engine/ui/components/ScrollBoxComponent.h"

namespace se::ui::observers
{
    class ScrollBoxObserver : public ecs::Observer<components::ScrollBoxComponent>
    {
        void OnAdded(const ecs::Id& entity, components::ScrollBoxComponent* component) override;
        void OnRemoved(const ecs::Id& entity, components::ScrollBoxComponent* component) override;
    };
}
