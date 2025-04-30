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
    DECLARE_SPARK_WIDGET_COMPONENT(HorizontalBoxComponent)

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
                                                                                                 const ui::components::RectTransformComponent&,
                                                                                                 const ui::components::RectTransformComponent& thisRect,
                                                                                                 const components::HorizontalBoxComponent* context)
    {
        return GetHorizontalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }
}
