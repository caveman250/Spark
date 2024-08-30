#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/TitleBarComponent.h"

namespace se::ui::observers
{
    class TitleBarObserver : public ecs::Observer<components::TitleBarComponent>
    {
        void OnAdded(ecs::Id entity, components::TitleBarComponent *component) override;
        void OnRemoved(ecs::Id entity, components::TitleBarComponent *component) override;
    };
}
