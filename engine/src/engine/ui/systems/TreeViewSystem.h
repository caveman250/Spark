#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/TreeViewComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::components {
    struct TreeNodeComponent;
}

namespace se::ui::systems
{
    class TreeViewSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
