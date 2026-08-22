module;

#include "engine/ui/Layout.h"

export module SplitViewComponent;
import Signal;
import DesiredSizeCalculator;
import Reflect;

namespace se::ui::components
{
    SPARK_ENUM()
    export enum class SplitViewDirection
    {
        Vertical,
        Horizontal
    };

    export struct SplitViewComponent : ecs::Component
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
    export template<>
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
