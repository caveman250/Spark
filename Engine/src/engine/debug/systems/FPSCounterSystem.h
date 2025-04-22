#pragma once
#include "engine/ecs/System.h"

namespace se::debug::systems
{
    class FPSCounterSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(FPSCounterSystem)

        void OnInit(const ecs::SystemUpdateData&) override;
        void OnUpdate(const ecs::SystemUpdateData&) override;
    };
}
