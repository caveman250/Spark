#pragma once

#include "engine/ecs/Observer.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::observers
{
    class RectTransformObserver : public ecs::Observer<components::RectTransformComponent>
    {
        void OnAdded(ecs::Id entity, components::RectTransformComponent *component) override;
        void OnRemoved(ecs::Id entity, components::RectTransformComponent *component) override;
    };
}
