#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ecs/components/TransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    class RootTransformSystem : public EngineSystem<TransformComponent, const RootComponent>
    {
        DECLARE_SPARK_SYSTEM(RootTransformSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities, TransformComponent* transform, const RootComponent* root) override;
    };
}
