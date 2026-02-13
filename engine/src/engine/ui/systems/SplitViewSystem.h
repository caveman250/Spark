#pragma once

#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class SplitViewSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
