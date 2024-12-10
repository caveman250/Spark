#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::systems
{
    class UIRenderSystem
: public ecs::EngineSystem<const ecs::components::RootComponent,
                    const components::WidgetComponent,
                    singleton_components::UIRenderComponent>
    {
        DECLARE_SPARK_SYSTEM(UIRenderSystem)

    public:
        void OnUpdate(const std::vector<ecs::Id> &,
                      const ecs::components::RootComponent *,
                      const components::WidgetComponent *,
                      singleton_components::UIRenderComponent *) override;

    };
}
