#pragma once

#include "util/RectTransformUtil.h"
#include "spark.h"
#include "engine/ecs/System.h"

namespace se::ui::components
{
    struct TextComponent;
}

namespace se::ui
{
    class Layout
    {
        static void DoLayoutChildren(ecs::World* world,
                                         ecs::System* system,
                                         const ecs::Id& entity,
                                         int layer,
                                         components::RectTransformComponent& thisRect);

    public:
        template<typename T>
        static void LayoutWidgetChildren(ecs::World* world,
                                         ecs::System* system,
                                         const ecs::Id& entity,
                                         components::RectTransformComponent& thisRect,
                                         int layer,
                                         T*)
        {
            EASY_FUNCTION();
            DoLayoutChildren(world, system, entity, layer, thisRect);
        }
    };
}
