#pragma once

#include "spark.h"
#include "engine/ui/components/ScrollViewComponent.h"
#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
class ScrollViewUpdateSystem : public ecs::EngineSystem
    {
    DECLARE_SPARK_SYSTEM(ScrollViewUpdateSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
