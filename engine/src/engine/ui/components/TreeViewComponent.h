#pragma once

#include "spark.h"
#include "engine/ui/Rect.h"
#include "engine/ecs/Component.h"
#include "engine/ui/Layout.h"

namespace se::ui::components
{
    struct TreeViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        bool dirty = false;
        Rect lastRect = {};
    };
}

namespace se::ui
{
    template<>
    void Layout::LayoutWidgetChildren<components::TreeViewComponent>(ecs::World* world,
                                                                 ecs::System* system,
                                                                 const ecs::Id& entity,
                                                                 components::RectTransformComponent& splitViewTransform,
                                                                 components::TreeViewComponent* splitView);
}
