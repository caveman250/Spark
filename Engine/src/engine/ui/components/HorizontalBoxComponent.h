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
    struct HorizontalBoxComponent : reflect::ObjectBase
    {
    SPARK_WIDGET_COMPONENT(HorizontalBoxComponent)

        int spacing = 0;
        bool dirty = false;
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    math::IntVec2 GetHorizontalBoxChildrenDesiredSize(ecs::System* system,
                                                    const ecs::Id& entity,
                                                    const ui::components::RectTransformComponent& thisRect,
                                                    const components::HorizontalBoxComponent* context);

    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::HorizontalBoxComponent>(ecs::System* system,
                                                                                                 const ecs::Id& entity,
                                                                                                 const ui::components::RectTransformComponent& parentRect,
                                                                                                 ui::components::RectTransformComponent& thisRect,
                                                                                                 const components::HorizontalBoxComponent* context)
    {
        if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
        {
            // need to calculate rect for children to calculate their desired size.
            thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
        }

        return CalculateAnchorOffsets(thisRect, parentRect.rect) +
                GetHorizontalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }
}
