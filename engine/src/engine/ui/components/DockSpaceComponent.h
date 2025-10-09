#pragma once
#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ui::components
{
    SPARK_ENUM()
    enum class DockSpaceDirection
    {
        None,
        TopBottom,
        LeftRight
    };

    struct DockSpaceComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Id entityOne = {};
        ecs::Id splitterEntity = {};
        ecs::Id entityTwo = {};
        DockSpaceDirection direction = DockSpaceDirection::None;
    };
}

namespace se::ui
{
    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::DockSpaceComponent>(ecs::System*,
                                                                                                 const ecs::Id&,
                                                                                                 components::RectTransformComponent&,
                                                                                                 const components::DockSpaceComponent*)
    {
        // TODO
        return math::IntVec2();
    }
}
