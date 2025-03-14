#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ResetInputSystem : public ecs::EngineSystem<const components::RectTransformComponent, components::ReceivesMouseEventsComponent, input::InputComponent>
    {
        DECLARE_SPARK_SYSTEM(ResetInputSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>&, const components::RectTransformComponent*, components::ReceivesMouseEventsComponent*, input::InputComponent*) override;
    };
}
