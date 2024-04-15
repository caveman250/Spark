#include "Vec4.h"

#include "math.h"

namespace se::math
{
    Vec4 Vec4::operator+(const Vec4& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }

    Vec4 Vec4::operator-(const Vec4& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }

    Vec4 Vec4::operator*(const Vec4& rhs) const
    {
        return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
    }

    Vec4 Vec4::operator*(float scalar) const
    {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }

    Vec4 Vec4::operator/(const Vec4& rhs) const
    {
        return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
    }

    Vec4 Vec4::operator/(float scalar) const
    {
        const float div = 1.f / scalar;
        return { x * div, y * div, z * div, w * div };
    }

    bool Vec4::operator==(const Vec4& rhs) const
    {
        return FloatEqual(x, rhs.x) && FloatEqual(y, rhs.y) && FloatEqual(z, rhs.z) && FloatEqual(w, rhs.w);
    }

    bool Vec4::operator!=(const Vec4& rhs) const
    {
        return !(*this == rhs);
    }
}
