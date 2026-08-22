module;

#include "engine/ui/Layout.h"

export module CollapsingHeaderComponent;
import Signal;
import DesiredSizeCalculator;
import Reflect;

namespace se::ui::components
{
    export struct CollapsingHeaderComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<bool> onCollapsed = {};
        bool collapsed = true;
        bool lastCollapsed = false;
        size_t fontSize = 16;
        ecs::Id titleEntity = {};
        ecs::Id titleButton = {};
        ecs::Id contentsEntity = {};
    };
}

namespace se::ui
{
    export template <>
    math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::CollapsingHeaderComponent>(ecs::System* system,
                                                                                                 const ecs::Id&,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::CollapsingHeaderComponent* context);

    template <>
    void Layout::LayoutWidgetChildren<components::CollapsingHeaderComponent>(ecs::World* world,
                                     ecs::System* system,
                                     const ecs::Id& entity,
                                     components::RectTransformComponent& comboBoxRect,
                                     int layer,
                                     components::CollapsingHeaderComponent* comboBox);
}
