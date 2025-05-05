#pragma once

#include "engine/ui/util/RectTransformUtil.h"
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
                                                const components::RectTransformComponent& parentRect,
                                                components::RectTransformComponent& thisRect,
                                                const T*)
        {
            if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
            {
                // need to calculate rect for children to calculate their desired size.
                thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
            }

            auto rectSize = GetDesiredSizeFromRect(thisRect);
            auto childrenSize = CalculateAnchorOffsets(thisRect, parentRect.rect) + GetChildrenDesiredSize(system, entity, thisRect);
            return math::IntVec2(std::max(rectSize.x, childrenSize.x), std::max(rectSize.y, childrenSize.y));
        }

        static math::IntVec2 GetDesiredSizeFromRect(const components::RectTransformComponent& transform)
        {
            math::IntVec2 ret = {};
            if (transform.anchors.left == transform.anchors.right)
            {
                ret.x = std::abs(transform.maxX - transform.minX) + transform.minX;
            }
            else if (transform.anchors.right == 0)
            {
                ret.x = transform.maxX;
            }
            else if (transform.anchors.left == 0)
            {
                ret.x = transform.minX;
            }

            if (transform.anchors.top == transform.anchors.bottom)
            {
                ret.y = std::abs(transform.maxY - transform.minY) + transform.minY;
            }
            else if (transform.anchors.bottom == 0)
            {
                ret.y = transform.maxY;
            }
            else if (transform.anchors.top == 0)
            {
                ret.y = transform.minY;
            }

            return ret;
        }

        static math::IntVec2 CalculateAnchorOffsets(const components::RectTransformComponent& transform,
                                                    const Rect& parentRect)
        {
            math::IntVec2 ret = {};
            if (transform.anchors.left > 0.5f)
            {
                ret.x = static_cast<int>((transform.anchors.left * parentRect.size.x) - transform.minX);
            }
            else
            {
                ret.x = static_cast<int>((transform.anchors.left * parentRect.size.x) + transform.minX);
            }

            if (transform.anchors.top > 0.5f)
            {
                ret.y = static_cast<int>((transform.anchors.top * parentRect.size.y) - transform.minY);
            }
            else
            {
                ret.y = static_cast<int>((transform.anchors.top * parentRect.size.y) + transform.minY);
            }

            return ret;
        }

        static math::IntVec2 GetChildrenDesiredSize(ecs::System* system,
                                                 const ecs::Id& entity,
                                                 components::RectTransformComponent&);
    };
}