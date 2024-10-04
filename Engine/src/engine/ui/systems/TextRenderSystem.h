#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class TextRenderSystem : public ecs::EngineSystem<const components::RectTransformComponent, components::TextComponent, const components::WidgetComponent>
    {
        DECLARE_SPARK_SYSTEM(TextRenderSystem)
    public:
        void OnRender(const std::vector<ecs::Id>& entities, const components::RectTransformComponent* transform, components::TextComponent*, const components::WidgetComponent*) override;
    };
}
