#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class TextRenderSystem
            : public ecs::EngineSystem
    {
    DECLARE_SPARK_SYSTEM(TextRenderSystem)

    public:
        void OnRender(const ecs::SystemUpdateData& updateData) override;
    };
}
