module;

#include "spark.h"

export module CollisionSystem;
import System;

namespace se::geo::systems
{
    class CollisionSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
