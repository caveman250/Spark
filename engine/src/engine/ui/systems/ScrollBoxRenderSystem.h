#pragma once

#include "spark.h"
#include "engine/ui/components/ScrollBoxComponent.h"
#include "engine/ecs/System.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
class ScrollBoxRenderSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnRender(const ecs::SystemUpdateData& updateData) override;
    };
}
