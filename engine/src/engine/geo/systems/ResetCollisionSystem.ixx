module;

#include "spark.h"

export module ResetCollisionSystem;
import System;

namespace se::geo::systems
{
    class ResetCollisionSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
