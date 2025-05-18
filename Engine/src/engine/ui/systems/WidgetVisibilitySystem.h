#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ecs/components/RootComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class WidgetVisibilitySystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM(WidgetVisibilitySystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
