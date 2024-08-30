#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class TitleBarSystem : public ecs::EngineSystem<components::RectTransformComponent,
                                                    components::TitleBarComponent,
                                                    input::InputComponent,
                                                    const components::ReceivesMouseEventsComponent>
    {
        DECLARE_SPARK_SYSTEM(TitleBarSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities, components::RectTransformComponent*, components::TitleBarComponent*, input::InputComponent*, const components::ReceivesMouseEventsComponent*) override;
    };
}
