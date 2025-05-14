#include "FloatRect.h"

namespace se::ui
{
    DEFINE_SPARK_POD_CLASS_BEGIN(FloatRect)
        DEFINE_SERIALIZED_MEMBER(topLeft)
        DEFINE_SERIALIZED_MEMBER(size)
    DEFINE_SPARK_CLASS_END(FloatRect)

    bool FloatRect::Contains(const math::Vec2 &point) const
    {
        return point.x >= topLeft.x && point.y >= topLeft.y &&
                point.x <= topLeft.x + size.x && point.y <= topLeft.y + size.y;
    }

    bool FloatRect::Overlaps(const FloatRect &bb) const
    {
        math::Vec2 bottomRight = topLeft + size;
        math::Vec2 otherBottomRight = bb.topLeft + bb.size;

        return bb.topLeft.x <= bottomRight.x && otherBottomRight.x >= topLeft.x &&
                bb.topLeft.y <= bottomRight.y && otherBottomRight.y >= topLeft.y;
    }

}
