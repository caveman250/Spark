#include "FloatRect.h"

namespace se::ui
{
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
