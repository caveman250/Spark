export module DockSpaceComponent;

import DesiredSizeCalculator;

namespace se::ui::components
{
    SPARK_ENUM()
    export enum class DockSpaceDirection
    {
        None,
        TopBottom,
        LeftRight
    };

    export struct DockSpaceComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        ecs::Id entityOne = {};
        ecs::Id splitterEntity = {};
        ecs::Id entityTwo = {};
        DockSpaceDirection direction = DockSpaceDirection::None;
    };
}

export namespace se::ui
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
