#pragma once

#include "engine/ui/Rect.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ecs/System.h"

namespace se::ui
{
    class DesiredSizeCalculator
    {
    public:
        template<typename T>
        static inline math::IntVec2 GetDesiredSize(ecs::System* system,
                                                const ecs::Id& entity,
                                                const components::RectTransformComponent&,
                                                const components::RectTransformComponent& thisRect,
                                                const T*)
        {
            auto rectSize = GetDesiredSizeFromRect(thisRect);
            auto childrenSize = GetChildrenDesiredSize(system, entity, thisRect);
            return math::IntVec2(std::max(rectSize.x, childrenSize.x), std::max(rectSize.y, childrenSize.y));
        }

        static math::IntVec2 GetDesiredSizeFromRect(const components::RectTransformComponent& transform)
        {
            math::IntVec2 ret = {};
            if (transform.anchors.left == transform.anchors.right)
            {
                ret.x = std::abs(transform.maxX - transform.minX);
            }

            if (transform.anchors.top == transform.anchors.bottom)
            {
                ret.y = std::abs(transform.maxY - transform.minY);
            }

            return ret;
        }

        static math::IntVec2 GetChildrenDesiredSize(ecs::System* system,
                                                 const ecs::Id& entity,
                                                 const components::RectTransformComponent&);
    };
}