#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::systems
{
    class UIRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnRender(const ecs::QueryResults&) override;

    };
}
