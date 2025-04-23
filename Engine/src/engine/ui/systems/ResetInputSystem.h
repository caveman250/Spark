#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ResetInputSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(ResetInputSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
