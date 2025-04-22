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
        DECLARE_SPARK_WIDGET_COMPONENT(VerticalBoxComponent)

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
                                                                                              const ui::components::RectTransformComponent&,
                                                                                              const ui::components::RectTransformComponent& thisRect,
                                                                                              const components::VerticalBoxComponent* context)
    {
        return GetVerticalBoxChildrenDesiredSize(system, entity, thisRect, context);
    }
}
