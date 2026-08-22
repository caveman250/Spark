module;

#include "spark.h"
#include "engine/ecs/Component.h"

#include "engine/ui/Layout.h"

export module GridBoxComponent;
import System;

namespace se::ui::components
{
    export struct GridBoxComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        int spacing = 0;
    };
}

export namespace se::ui
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
