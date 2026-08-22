module;

#include "spark.h"
#include "engine/ecs/Component.h"

#include "engine/ui/Layout.h"

export module VerticalBoxComponent;
import DesiredSizeCalculator;

namespace se::ui::components
{
    export struct VerticalBoxComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        float paddingTop = 0;
        float paddingBottom = 0;
        float paddingLeft = 0;
        float paddingRight = 0;
        float spacing = 0;
    };
}

namespace se::ui
{
    math::IntVec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system,
                                                 const ecs::Id& entity,
                                                 const components::RectTransformComponent& thisRect,
                                                 const components::VerticalBoxComponent* context);

    export template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::VerticalBoxComponent>(ecs::System* system,
                                                                                              const ecs::Id& entity,
                                                                                              components::RectTransformComponent& thisRect,
                                                                                              const components::VerticalBoxComponent* context)
    {
        return GetVerticalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }


    export template <>
    void Layout::LayoutWidgetChildren<components::VerticalBoxComponent>(ecs::World*,
                                                         ecs::System*,
                                                         const ecs::Id&,
                                                         components::RectTransformComponent&,
                                                         int,
                                                         components::VerticalBoxComponent*);
}
