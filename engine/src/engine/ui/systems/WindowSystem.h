#pragma once

#include "engine/ecs/System.h"
#include "engine/input/InputComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class WindowSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
