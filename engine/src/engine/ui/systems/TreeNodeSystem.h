#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TreeNodeComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

namespace se::ui::components {
    struct TreeNodeComponent;
}

namespace se::ui::systems
{
class TreeNodeSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
