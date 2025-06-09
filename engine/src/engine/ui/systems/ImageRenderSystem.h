#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ImageRenderSystem
            : public ecs::EngineSystem
    {
    SPARK_SYSTEM()

    public:
        void OnRender(const ecs::SystemUpdateData& updateData) override;
    };
}
