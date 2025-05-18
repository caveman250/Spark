#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/VerticalBoxComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class HorizontalBoxSystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM(HorizontalBoxSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
