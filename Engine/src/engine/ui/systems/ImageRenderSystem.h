#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ImageRenderSystem
            : public ecs::EngineSystem
    {
    SPARK_SYSTEM(ImageRenderSystem)

    public:
        void OnRender(const ecs::SystemUpdateData& updateData) override;
    };
}
