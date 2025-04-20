#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/VerticalBoxComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class VerticalBoxSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(VerticalBoxSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
