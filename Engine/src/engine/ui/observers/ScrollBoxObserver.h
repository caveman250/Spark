#pragma once

#include "engine/ecs/Observer.h"
#include "engine/ui/components/ScrollBoxComponent.h"

namespace se::ui::observers
{
    class ScrollBoxObserver : public ecs::Observer<components::ScrollBoxComponent>
    {
        void OnAdded(ecs::Id entity, components::ScrollBoxComponent* component) override;
        void OnRemoved(ecs::Id entity, components::ScrollBoxComponent* component) override;
    };
}
