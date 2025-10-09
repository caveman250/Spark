#pragma once

#include "engine/ecs/System.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class HorizontalBoxSystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
