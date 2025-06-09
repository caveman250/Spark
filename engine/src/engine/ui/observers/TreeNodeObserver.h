#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/TreeNodeComponent.h"

namespace se::ui::observers
{
    class TreeNodeObserver : public ecs::Observer<components::TreeNodeComponent>
    {
        void OnAdded(const ecs::Id& entity, components::TreeNodeComponent* component) override;
        void OnRemoved(const ecs::Id& entity, components::TreeNodeComponent* component) override;
    };
}
