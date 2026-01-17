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
    struct GridBoxComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        int spacing = 0;
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

    template <>
    void Layout::LayoutWidgetChildren<components::GridBoxComponent>(ecs::World* world,
                                         ecs::System* system,
                                         const ecs::Id& entity,
                                         components::RectTransformComponent& gridBoxTransform,
                                         int layer,
                                         components::GridBoxComponent* gridBox);
}
