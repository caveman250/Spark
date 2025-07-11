#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/TransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    class TransformSystem : public EngineSystem
    {
        SPARK_SYSTEM()
    public:
        void OnUpdate(const SystemUpdateData& updateData) override;
    };
}
