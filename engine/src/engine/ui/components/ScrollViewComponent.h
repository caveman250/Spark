#pragma once

#include "engine/ecs/Signal.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/Layout.h"
#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui::components
{
    struct ScrollViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<const RectTransformComponent*, float> onScrolled = {};
        float scrollAmount = 0.f;
    };
}

namespace se::ui
{
    template<>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::ScrollViewComponent>(ecs::System* system,
                                                                       const ecs::Id&,
                                                                       components::RectTransformComponent& thisRect,
                                                                       const components::ScrollViewComponent* context);

    template<>
    void Layout::LayoutWidgetChildren<components::ScrollViewComponent>(ecs::World*,
                                                         ecs::System* system,
                                                         const ecs::Id& entity,
                                                         components::RectTransformComponent& rectTransform,
                                                         int layer,
                                                         components::ScrollViewComponent* treeView);
}
