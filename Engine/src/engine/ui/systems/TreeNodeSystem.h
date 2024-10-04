#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"

namespace se::ui::components {
    struct TreeNodeComponent;
}

namespace se::ui::systems
{
class TreeNodeSystem : public ecs::EngineSystem<components::TreeNodeComponent, components::ReceivesMouseEventsComponent>
    {
        DECLARE_SPARK_SYSTEM(TreeNodeSystem)

        void OnUpdate(const std::vector<ecs::Id>&, components::TreeNodeComponent*, components::ReceivesMouseEventsComponent*) override;
    };
}
