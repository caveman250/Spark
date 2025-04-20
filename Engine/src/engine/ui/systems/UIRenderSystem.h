#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::systems
{
    class UIRenderSystem
: public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(UIRenderSystem)

    public:
        void OnRender(const ecs::SystemUpdateData& updateData) override;

    };
}
