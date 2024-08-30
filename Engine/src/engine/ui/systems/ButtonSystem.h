#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ButtonSystem : public ecs::EngineSystem<components::ButtonComponent,
                                                    components::ImageComponent,
                                                    const components::ReceivesMouseEventsComponent>
    {
        DECLARE_SPARK_SYSTEM(ButtonSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities,
            components::ButtonComponent*,
            components::ImageComponent*,
            const components::ReceivesMouseEventsComponent*) override;
    };
}
