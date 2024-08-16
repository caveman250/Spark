#pragma once

#include "spark.h"
#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/render/components/PointLightComponent.h"

namespace se::render::systems
{
    class PointLightSystem : public ecs::EngineSystem<const components::PointLightComponent, const ecs::components::TransformComponent>
    {
        DECLARE_SPARK_SYSTEM(PointLightSystem)

        void OnUpdate(const std::vector<se::ecs::Id>&, const components::PointLightComponent*, const ecs::components::TransformComponent*) override;
    };
}
