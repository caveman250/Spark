module;

#include "engine/ecs/Observer.h"

export module TreeNodeObserver;
import TreeNodeComponent;

namespace se::ui::observers
{
    export class TreeNodeObserver : public ecs::Observer<components::TreeNodeComponent>
    {
        void OnAdded(const ecs::Id& entity, components::TreeNodeComponent* component) override;
        void OnRemoved(const ecs::Id& entity, components::TreeNodeComponent* component) override;
    };
}
