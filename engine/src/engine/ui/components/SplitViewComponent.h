#pragma once

#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/Layout.h"

namespace se::ui::components
{
    SPARK_ENUM()
    enum class SplitViewDirection
    {
        Vertical,
        Horizontal
    };

    struct SplitViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Id entity1 = { };
        ecs::Id sliderEntity = { };
        ecs::Id entity2 = { };
        float sliderPos = .5f;
        float lastSliderPos = { };
        bool isResizing = false;
        bool isShowingResizeCursor = false;
        SplitViewDirection dir = SplitViewDirection::Horizontal;
    };
}

namespace se::ui
{
    template<>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::SplitViewComponent>(ecs::System* system,
                                                                                               const ecs::Id&,
                                                                                               components::RectTransformComponent& thisRect,
                                                                                               const components::SplitViewComponent* context);

    template<>
    void Layout::LayoutWidgetChildren<components::SplitViewComponent>(ecs::World* world,
                                                                     ecs::System* system,
                                                                     const ecs::Id& entity,
                                                                     components::RectTransformComponent& splitViewTransform,
                                                                     int layer,
                                                                     components::SplitViewComponent* splitView);
}
