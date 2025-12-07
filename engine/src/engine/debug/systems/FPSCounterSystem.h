#pragma once
#include "engine/ecs/System.h"

#if WITH_DEV_ONLY_CLASSES
namespace se::debug::systems
{
    class FPSCounterSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM(DevOnly)

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnInit(const ecs::SystemUpdateData&) override;
        void OnUpdate(const ecs::SystemUpdateData&) override;
    };
}
#endif
