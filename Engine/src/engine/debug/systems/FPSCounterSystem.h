#pragma once
#include "engine/ecs/System.h"

namespace se::debug::systems
{
    class FPSCounterSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        void OnInit(const ecs::SystemUpdateData&) override;
        void OnUpdate(const ecs::SystemUpdateData&) override;
    };
}
