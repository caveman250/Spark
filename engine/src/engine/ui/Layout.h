#pragma once

#include "util/RectTransformUtil.h"
#include "spark.h"
#include "engine/ecs/System.h"

namespace se::ui
{
    class Layout
    {
        static void DoLayoutChildren(ecs::World* world,
                                         ecs::System* system,
                                         const ecs::Id& entity,
                                         components::RectTransformComponent& thisRect);

    public:
        template<typename T>
        static void LayoutWidgetChildren(ecs::World* world,
                                         ecs::System* system,
                                         const ecs::Id& entity,
                                         components::RectTransformComponent& thisRect,
                                         T*)
        {
            EASY_FUNCTION();
            DoLayoutChildren(world, system, entity, thisRect);
        }
    };
}
