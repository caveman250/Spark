module;

#include "spark.h"

export module AABBColliderSystem;
import System;

namespace se::geo::systems
{
    class AABBColliderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
