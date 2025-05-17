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
    struct GridBoxComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(GridBoxComponent)

        int spacing = 0;
        bool dirty = false;
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    se::math::IntVec2 GetGridBoxChildrenDesiredSize(ecs::System* system,
                                                    const ecs::Id& entity,
                                                    const ui::components::RectTransformComponent& parentRect,
                                                    const ui::components::RectTransformComponent& thisRect,
                                                    const components::GridBoxComponent* context);

    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::GridBoxComponent>(ecs::System* system,
                                                                                                 const ecs::Id& entity,
                                                                                                 const ui::components::RectTransformComponent& parentRect,
                                                                                                 ui::components::RectTransformComponent& thisRect,
                                                                                                 const components::GridBoxComponent* context)
    {
        if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
        {
            // need to calculate rect for children to calculate their desired size.
            thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
        }

        return CalculateAnchorOffsets(thisRect, parentRect.rect) + GetGridBoxChildrenDesiredSize(system, entity, parentRect, thisRect, context);
    }
}
