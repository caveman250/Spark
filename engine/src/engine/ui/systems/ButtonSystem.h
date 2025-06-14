#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ui/components/ButtonComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ButtonSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
