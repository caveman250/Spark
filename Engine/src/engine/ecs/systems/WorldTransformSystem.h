#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/components/ParentComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    class WorldTransformSystem : public EngineSystem<TransformComponent, ParentComponent>
    {
        DECLARE_SPARK_SYSTEM(WorldTransformSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities, TransformComponent* transform, ParentComponent* parentComp) override;
    };
}
