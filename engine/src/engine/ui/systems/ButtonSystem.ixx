module;

#include "engine/ecs/SystemDeclaration.h"
#include "engine/ecs/World.h"

export module ButtonSystem;

import System;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class ButtonSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;
    };
}
