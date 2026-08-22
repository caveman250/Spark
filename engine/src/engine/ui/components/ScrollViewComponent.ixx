module;

#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/Layout.h"

export module ScrollViewComponent;
import Signal;
import DesiredSizeCalculator;

namespace se::ui::components
{
    export struct ScrollViewComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<const RectTransformComponent*, float> onScrolled = {};
        float scrollAmount = 0.f;
    };
}

namespace se::ui
{
    export template<>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::ScrollViewComponent>(ecs::System* system,
                                                                       const ecs::Id&,
                                                                       components::RectTransformComponent& thisRect,
                                                                       const components::ScrollViewComponent* context);

    export template<>
    void Layout::LayoutWidgetChildren<components::ScrollViewComponent>(ecs::World*,
                                                         ecs::System* system,
                                                         const ecs::Id& entity,
                                                         components::RectTransformComponent& rectTransform,
                                                         int layer,
                                                         components::ScrollViewComponent* treeView);
}
