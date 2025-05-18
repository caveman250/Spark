#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Rect.h"
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

        int paddingTop = 0;
        int paddingBottom = 0;
        int paddingLeft = 0;
        int paddingRight = 0;
        int spacing = 0;
        bool dirty = false;
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    math::IntVec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system,
                                                 const ecs::Id& entity,
                                                 const ui::components::RectTransformComponent& thisRect,
                                                 const components::VerticalBoxComponent* context);

    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::VerticalBoxComponent>(ecs::System* system,
                                                                                              const ecs::Id& entity,
                                                                                              const ui::components::RectTransformComponent& parentRect,
                                                                                              ui::components::RectTransformComponent& thisRect,
                                                                                              const components::VerticalBoxComponent* context)
    {
        if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
        {
            // need to calculate rect for children to calculate their desired size.
            thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
        }

        return CalculateAnchorOffsets(thisRect, parentRect.rect) +
            GetVerticalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }
}
