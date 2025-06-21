#pragma once

#include "components/WidgetComponent.h"
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
                                                components::RectTransformComponent& thisRect,
                                                const T*)
        {
            auto window = Application::Get()->GetWindow();
            math::IntVec2 rectSize = GetDesiredSizeFromRect(thisRect);
            auto childrenSize = GetChildrenDesiredSize(system, entity, thisRect);
            return math::IntVec2(std::max(std::max(rectSize.x, childrenSize.x), static_cast<int>(thisRect.minWidth * window->GetContentScale())),
                                 std::max(std::max(rectSize.y, childrenSize.y), static_cast<int>(thisRect.minHeight * window->GetContentScale())));
        }

        template<typename T>
        static inline math::IntVec2 GetDesiredSize(ecs::System* system,
                                                const ecs::Id& entity,
                                                const components::WidgetComponent* widget,
                                                const components::RectTransformComponent& parentRect,
                                                components::RectTransformComponent& thisRect,
                                                const T* context)
        {
            if (widget && widget->visibility == Visibility::Collapsed)
            {
                return {};
            }

            if (thisRect.rect.size.x == 0 && thisRect.rect.size.y == 0)
            {
                // need to calculate rect for children to calculate their desired size.
                thisRect.rect = util::CalculateScreenSpaceRect(thisRect, parentRect);
            }

            auto anchorOffsets = CalculateAnchorOffsets(thisRect, parentRect.rect);
            auto desired = GetDesiredSize(system, entity, thisRect, context);
            thisRect.desiredSize = anchorOffsets + desired;
            return anchorOffsets + desired;
        }

        static math::IntVec2 GetDesiredSizeFromRect(const components::RectTransformComponent& transform)
        {
            math::IntVec2 ret = {};
            auto window = Application::Get()->GetWindow();
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
            auto window = Application::Get()->GetWindow();
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