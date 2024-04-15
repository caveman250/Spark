#include "Vec3.h"

#include "math.h"

namespace se::math
{
    Vec3 Vec3::operator+(const Vec3& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    Vec3 Vec3::operator-(const Vec3& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    Vec3 Vec3::operator*(const Vec3& rhs) const
    {
        return { x * rhs.x, y * rhs.y, z * rhs.z };
    }

    Vec3 Vec3::operator*(float scalar) const
    {
        return { x * scalar, y * scalar, z * scalar };
    }

    Vec3 Vec3::operator/(const Vec3& rhs) const
    {
        return { x / rhs.x, y / rhs.y, z / rhs.z };
    }

    Vec3 Vec3::operator/(float scalar) const
    {
        const float div = 1.f / scalar;
        return { x * div, y * div, z * div };
    }

    bool Vec3::operator==(const Vec3& rhs) const
    {
        return FloatEqual(x, rhs.x) && FloatEqual(y, rhs.y) && FloatEqual(z, rhs.z);
    }

    bool Vec3::operator!=(const Vec3& rhs) const
    {
        return !(*this == rhs);
    }
}
