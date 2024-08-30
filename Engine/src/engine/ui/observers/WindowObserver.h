#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/WindowComponent.h"

namespace se::ui::observers
{
    class WindowObserver : public ecs::Observer<components::WindowComponent>
    {
        void OnAdded(ecs::Id entity, components::WindowComponent *component) override;
        void OnRemoved(ecs::Id entity, components::WindowComponent *component) override;
    };
}
