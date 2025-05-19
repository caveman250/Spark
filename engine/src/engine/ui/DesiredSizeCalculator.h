#pragma once

#include "engine/ui/util/RectTransformUtil.h"
#include "engine/ui/Rect.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ecs/System.h"
#include "platform/IWindow.h"

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
            auto window = Application::Get()->GetPrimaryWindow();
            if (transform.anchors.left == transform.anchors.right)
            {
                ret.x = static_cast<int>(std::abs(transform.maxX * window->GetContentScale() - transform.minX * window->GetContentScale()) + transform.minX * window->GetContentScale());
            }
            else if (transform.anchors.right == 0)
            {
                ret.x = static_cast<int>(transform.maxX * window->GetContentScale());
            }
            else if (transform.anchors.left == 0)
            {
                ret.x = static_cast<int>(transform.minX * window->GetContentScale());
            }

            if (transform.anchors.top == transform.anchors.bottom)
            {
                ret.y = static_cast<int>(std::abs(transform.maxY * window->GetContentScale() - transform.minY * window->GetContentScale()) + transform.minY * window->GetContentScale());
            }
            else if (transform.anchors.bottom == 0)
            {
                ret.y = static_cast<int>(transform.maxY * window->GetContentScale());
            }
            else if (transform.anchors.top == 0)
            {
                ret.y = static_cast<int>(transform.minY * window->GetContentScale());
            }

            return ret;
        }

        static math::IntVec2 CalculateAnchorOffsets(const components::RectTransformComponent& transform,
                                                    const Rect& parentRect)
        {
            math::IntVec2 ret = {};
            auto window = Application::Get()->GetPrimaryWindow();
            if (transform.anchors.left > 0.5f)
            {
                ret.x = static_cast<int>((transform.anchors.left * parentRect.size.x) - transform.minX * window->GetContentScale());
            }
            else
            {
                ret.x = static_cast<int>((transform.anchors.left * parentRect.size.x) + transform.minX * window->GetContentScale());
            }

            if (transform.anchors.top > 0.5f)
            {
                ret.y = static_cast<int>((transform.anchors.top * parentRect.size.y) - transform.minY * window->GetContentScale());
            }
            else
            {
                ret.y = static_cast<int>((transform.anchors.top * parentRect.size.y) + transform.minY * window->GetContentScale());
            }

            return ret;
        }

        static math::IntVec2 GetChildrenDesiredSize(ecs::System* system,
                                                 const ecs::Id& entity,
                                                 components::RectTransformComponent&);
    };
}