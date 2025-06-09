#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::systems
{
    class UIRenderSystem
: public ecs::EngineSystem
    {
        SPARK_SYSTEM()

    public:
        void OnRender(const ecs::SystemUpdateData& updateData) override;

    };
}
