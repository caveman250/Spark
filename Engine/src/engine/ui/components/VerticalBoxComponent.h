#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/ui/Rect.h"
#include "engine/math/math.h"
#include "engine/ui/DesiredSizeCalculator.h"

namespace se::ecs
{
    class System;
}

namespace se::ui::components
{
    struct VerticalBoxComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(VerticalBoxComponent)
    };
}

namespace se::ui
{
    math::Vec2 GetVerticalBoxChildrenDesiredSize(ecs::System* system, const ecs::Id& entity, const Rect&);

    template <>
    inline math::Vec2 DesiredSizeCalculator::GetDesiredSize<components::VerticalBoxComponent>(ecs::System* system,
                                                                                              const ecs::Id& entity,
                                                                                              const Rect& parentRect,
                                                                                              const components::VerticalBoxComponent*)
    {
        return GetVerticalBoxChildrenDesiredSize(system, entity, parentRect);
    }
}
