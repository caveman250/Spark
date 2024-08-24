#include "Rect.h"

namespace se::ui
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Rect)
        DEFINE_MEMBER(topLeft)
        DEFINE_MEMBER(size)
    DEFINE_SPARK_CLASS_END()

    bool Rect::Contains(const math::IntVec2 &point) const
    {
        return point.x >= topLeft.x && point.y >= topLeft.y &&
                point.x <= topLeft.x + size.x && point.y <= topLeft.y + size.y;
    }

    bool Rect::Overlaps(const Rect &bb) const
    {
        math::IntVec2 bottomRight = topLeft + size;
        math::IntVec2 otherBottomRight = bb.topLeft + bb.size;

        return bb.topLeft.x <= bottomRight.x && otherBottomRight.x >= topLeft.x &&
                bb.topLeft.y <= bottomRight.y && otherBottomRight.y >= topLeft.y;
    }

}