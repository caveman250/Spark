#pragma once

#include "spark.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
class ScrollBoxSystem : public ecs::EngineSystem<components::ScrollBoxComponent, const components::RectTransformComponent, singleton_components::UIRenderComponent>
    {
        DECLARE_SPARK_SYSTEM(ScrollBoxSystem)
    public:
        void OnRender(const std::vector<ecs::Id>& entities,
                      components::ScrollBoxComponent*,
                      const components::RectTransformComponent*,
                      singleton_components::UIRenderComponent*) override;
    };
}
