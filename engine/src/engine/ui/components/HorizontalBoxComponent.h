#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/math/math.h"
#include "engine/ui/Layout.h"

namespace se::ecs
{
    class System;
}

namespace se::ui::components
{
    struct HorizontalBoxComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        float paddingTop = 0;
        float paddingBottom = 0;
        float paddingLeft = 0;
        float paddingRight = 0;
        int spacing = 0;
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    math::IntVec2 GetHorizontalBoxChildrenDesiredSize(ecs::System* system,
                                                    const ecs::Id& entity,
                                                    const components::RectTransformComponent& thisRect,
                                                    const components::HorizontalBoxComponent* context);

    template <>
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
                                                         components::HorizontalBoxComponent*);
}
