#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ecs/components/TransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    class RootTransformSystem : public EngineSystem
    {
        DECLARE_SPARK_SYSTEM(RootTransformSystem)
    public:
        void OnUpdate(const SystemUpdateData& updateData) override;
    };
}
