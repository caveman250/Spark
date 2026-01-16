#pragma once
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/DesiredSizeCalculator.h"
#include "engine/ui/Layout.h"

namespace se::ui::components
{
    struct CollapsingHeaderComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Signal<bool> onCollapsed = {};
        bool collapsed = true;
        bool lastCollapsed = false;
        size_t fontSize = 16;
        ecs::Id titleEntity = {};
        ecs::Id contentsEntity = {};
    };
}

namespace se::ui
{
    template <>
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
