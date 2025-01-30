#include "RectTransformUtil.h"

namespace se::ui::util
{
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform)
    {
        math::IntVec2 parentBottomRight = parentTransform.rect.topLeft + parentTransform.rect.size;
        int parentWidth = parentBottomRight.x - parentTransform.rect.topLeft.x;
        int parentHeight = parentBottomRight.y - parentTransform.rect.topLeft.y;

        Rect ret = {};
        if (transform.anchors.left > 0.5f)
        {
            ret.topLeft.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.left * parentWidth) - transform.minX);
        }
        else
        {
            ret.topLeft.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.left * parentWidth) + transform.minX);
        }

        if (transform.anchors.top > 0.5f)
        {
            ret.topLeft.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.top * parentHeight) - transform.minY);
        }
        else
        {
            ret.topLeft.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.top * parentHeight) + transform.minY);
        }

        if (transform.anchors.right > 0.5f)
        {
            ret.size.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.right * parentWidth) - transform.maxX - ret.topLeft.x);
        }
        else
        {
            ret.size.x = static_cast<int>((parentTransform.rect.topLeft.x + transform.anchors.right * parentWidth) + transform.maxX - ret.topLeft.x);
        }

        if (transform.anchors.bottom > 0.5f)
        {
            ret.size.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.bottom * parentHeight) - transform.maxY - ret.topLeft.y);
        }
        else
        {
            ret.size.y = static_cast<int>((parentTransform.rect.topLeft.y + transform.anchors.bottom * parentHeight) + transform.maxY - ret.topLeft.y);
        }
        return ret;
    }
}
