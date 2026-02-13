#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/RootComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    class RootTransformSystem : public EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const QueryResults&) override;
    };
}
