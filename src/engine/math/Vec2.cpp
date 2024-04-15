#include "Vec2.h"

#include "math.h"

namespace se::math
{
    Vec2 Vec2::operator+(const Vec2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    Vec2 Vec2::operator-(const Vec2& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    Vec2 Vec2::operator*(const Vec2& rhs) const
    {
        return { x * rhs.x, y * rhs.y };
    }

    Vec2 Vec2::operator*(float scalar) const
    {
        return { x * scalar, y * scalar };
    }

    Vec2 Vec2::operator/(const Vec2& rhs) const
    {
        return { x / rhs.x, y / rhs.y };
    }

    Vec2 Vec2::operator/(float scalar) const
    {
        const float div = 1.f / scalar;
        return { x * div, y * div };
    }

    bool Vec2::operator==(const Vec2& rhs) const
    {
        return FloatEqual(x, rhs.x) && FloatEqual(y, rhs.y);
    }

    bool Vec2::operator!=(const Vec2& rhs) const
    {
        return !(*this == rhs);
    }
}
