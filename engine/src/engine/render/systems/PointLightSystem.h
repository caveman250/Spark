#pragma once

#include "spark.h"
#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"

namespace se::render::systems
{
    class PointLightSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
