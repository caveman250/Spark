#pragma once

#include "engine/ecs/Observer.h"
#include "engine/ui/components/RectTransformComponent.h"

namespace se::ui::observers
{
    class RectTransformObserver : public ecs::Observer<components::RectTransformComponent>
    {
        void OnAdded(const ecs::Id& entity, components::RectTransformComponent *component) override;
        void OnRemoved(const ecs::Id& entity, components::RectTransformComponent *component) override;
    };
}
