#pragma once

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::geo::systems
{
    class AABBColliderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
