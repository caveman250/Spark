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
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
