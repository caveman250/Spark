#pragma once

#include "spark.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
class ScrollBoxUpdateSystem : public ecs::EngineSystem<components::ScrollBoxComponent, components::RectTransformComponent, const components::ReceivesMouseEventsComponent>
    {
    DECLARE_SPARK_SYSTEM(ScrollBoxUpdateSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities,
                      components::ScrollBoxComponent*,
                      components::RectTransformComponent*,
                      const components::ReceivesMouseEventsComponent*) override;
    };
}
