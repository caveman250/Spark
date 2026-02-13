#pragma once

#include "engine/ecs/System.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class RootRectTransformSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults& results) override;
    };
}
