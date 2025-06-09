#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/math/math.h"

namespace se::ecs
{
    class System;
}

namespace se::ui::components
{
    struct VerticalBoxComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        float paddingTop = 0;
        float paddingBottom = 0;
        float paddingLeft = 0;
        float paddingRight = 0;
        float spacing = 0;
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    math::IntVec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system,
                                                 const ecs::Id& entity,
                                                 const components::RectTransformComponent& thisRect,
                                                 const components::VerticalBoxComponent* context);

    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::VerticalBoxComponent>(ecs::System* system,
                                                                                              const ecs::Id& entity,
                                                                                              components::RectTransformComponent& thisRect,
                                                                                              const components::VerticalBoxComponent* context)
    {
        return GetVerticalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }
}
