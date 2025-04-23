#pragma once

#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class ResetKeyInputSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(ResetKeyInputSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
