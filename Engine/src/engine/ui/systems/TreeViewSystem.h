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
        DECLARE_SPARK_SYSTEM(TreeViewSystem)

        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    private:
        int MeasureAndArrange(ecs::Id entity,  bool collapsed, int startY);
    };
}
