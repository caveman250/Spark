module;

#include "spark.h"
#include "engine/ecs/SystemDeclaration.h"
#include "engine/ecs/World.h"

export module AABBColliderResolveSystem;
import System;

namespace se::geo::systems
{
    export class AABBColliderResolveSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
