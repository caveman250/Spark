#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/TitleBarComponent.h"

namespace se::ui::observers
{
    class TitleBarObserver : public ecs::Observer<components::TitleBarComponent>
    {
        void OnAdded(const ecs::Id& entity, components::TitleBarComponent *component) override;
        void OnRemoved(const ecs::Id& entity, components::TitleBarComponent *component) override;
    };
}
