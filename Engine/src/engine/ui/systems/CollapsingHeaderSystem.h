#pragma once

#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class CollapsingHeaderSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(CollapsingHeaderSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
