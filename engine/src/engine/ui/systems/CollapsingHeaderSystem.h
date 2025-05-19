#pragma once

#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class CollapsingHeaderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
