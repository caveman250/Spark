#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/ButtonComponent.h"

namespace se::ui::observers
{
    class ButtonObserver : public ecs::Observer<components::ButtonComponent>
    {
        void OnAdded(const ecs::Id& entity, components::ButtonComponent *component) override;
        void OnRemoved(const ecs::Id& entity, components::ButtonComponent *component) override;
    };
}
