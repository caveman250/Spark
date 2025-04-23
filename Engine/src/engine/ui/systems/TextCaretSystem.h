#pragma once

#include "spark.h"
#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class TextCaretSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(TextCaretSystem)

    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
