#pragma once

#include "spark.h"
#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/render/components/PointLightComponent.h"

namespace se::render::systems
{
    class PointLightSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
