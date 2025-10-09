#pragma once

#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class ResetKeyInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
