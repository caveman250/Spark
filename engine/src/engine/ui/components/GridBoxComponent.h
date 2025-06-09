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
    struct GridBoxComponent : reflect::ObjectBase
    {
        SPARK_WIDGET_COMPONENT()

        int spacing = 0;
        bool dirty = false;
    };
}

#include "engine/ui/DesiredSizeCalculator.h"
namespace se::ui
{
    math::IntVec2 GetGridBoxChildrenDesiredSize(ecs::System* system,
                                                    const ecs::Id& entity,
                                                    const components::RectTransformComponent& thisRect,
                                                    const components::GridBoxComponent* context);

    template <>
    inline math::IntVec2 DesiredSizeCalculator::GetDesiredSize<components::GridBoxComponent>(ecs::System* system,
                                                                                                 const ecs::Id& entity,
                                                                                                 components::RectTransformComponent& thisRect,
                                                                                                 const components::GridBoxComponent* context)
    {
        return GetGridBoxChildrenDesiredSize(system, entity, thisRect, context);
    }
}
