#pragma once

#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class ResetKeyInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
