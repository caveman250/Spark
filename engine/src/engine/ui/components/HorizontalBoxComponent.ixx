module;

#include "spark.h"
#include "engine/ecs/Component.h"

#include "engine/ui/Layout.h"

export module HorizontalBoxComponent;
import DesiredSizeCalculator;

namespace se::ui::components
{
    export struct HorizontalBoxComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        float paddingTop = 0;
        float paddingBottom = 0;
        float paddingLeft = 0;
        float paddingRight = 0;
        int spacing = 0;
    };
}

namespace se::ui
{
    export math::IntVec2 GetHorizontalBoxChildrenDesiredSize(ecs::System* system,
                                                    const ecs::Id& entity,
                                                    const components::RectTransformComponent& thisRect,
                                                    const components::HorizontalBoxComponent* context);

    export template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::HorizontalBoxComponent>(ecs::System* system,
                                                                                                 const ecs::Id& entity,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::HorizontalBoxComponent* context)
    {
        return GetHorizontalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }

    template <>
    void Layout::LayoutWidgetChildren<components::HorizontalBoxComponent>(ecs::World*,
                                                         ecs::System*,
                                                         const ecs::Id&,
                                                         components::RectTransformComponent&,
                                                         int layer,
                                                         components::HorizontalBoxComponent*);
}
