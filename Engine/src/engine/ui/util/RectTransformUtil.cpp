#include "RectTransformUtil.h"

namespace se::ui::util
{
    Rect CalculateScreenSpaceRect(const RectTransformComponent& transform, const RectTransformComponent& parentTransform)
    {
        math::IntVec2 parentBottomRight = parentTransform.rect.topLeft + parentTransform.rect.size;
        int parentWidth = parentBottomRight.x - parentTransform.rect.topLeft.x;
        int parentHeight = parentBottomRight.y - parentTransform.rect.topLeft.y;

        Rect ret = {};
        ret.topLeft = { static_cast<int>(parentTransform.rect.topLeft.x + transform.minX + transform.anchors.left * parentWidth),
                                  static_cast<int>(parentTransform.rect.topLeft.y + transform.minY + transform.anchors.top * parentHeight) };

        if (transform.anchors.right > 0)
        {
            ret.size.x = static_cast<int>(parentBottomRight.x - transform.maxX - ((1.f - transform.anchors.right) * (float)parentWidth) - ret.topLeft.x);
        }
        else
        {
            ret.size.x = parentTransform.rect.topLeft.x + transform.maxX - ret.topLeft.x;
        }

        if (transform.anchors.bottom > 0)
        {
            ret.size.y = static_cast<int>(parentBottomRight.y - transform.maxY - ((1.f - transform.anchors.bottom) * (float)parentHeight) - ret.topLeft.y);
        }
        else
        {
            ret.size.y = parentTransform.rect.topLeft.y + transform.maxY - ret.topLeft.y;
        }

        return ret;
    }
}
