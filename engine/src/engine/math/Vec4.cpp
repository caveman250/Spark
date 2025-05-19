#include "Vec4.h"

#include "math.h"

namespace se::math
{
    float& Vec4::operator[](size_t i)
    {
        SPARK_ASSERT(i <= 3u);
        switch(i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    const float& Vec4::operator[](size_t i) const
    {
        SPARK_ASSERT(i <= 3);
        switch(i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }

    Vec4& Vec4::operator+=(const Vec4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    Vec4& Vec4::operator+=(float scalar)
    {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    Vec4& Vec4::operator-=(const Vec4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    Vec4& Vec4::operator-=(float scalar)
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    Vec4& Vec4::operator*=(const Vec4& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }

    Vec4& Vec4::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    Vec4& Vec4::operator/=(const Vec4& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }

    Vec4& Vec4::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    Vec4 operator-(const Vec4& lhs)
    {
        return {-lhs.x, -lhs.y, -lhs.z, -lhs.w };
    }

    Vec4 operator+(const Vec4& lhs, const Vec4& rhs)
    {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
    }

    Vec4 operator-(const Vec4& lhs, const Vec4& rhs)
    {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
    }

    Vec4 operator*(const Vec4& lhs, const Vec4& rhs)
    {
        return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
    }

    Vec4 operator*(const Vec4& lhs, float scalar)
    {
        return { lhs.x * scalar, lhs.y * scalar, lhs.z * scalar, lhs.w * scalar };
    }

    Vec4 operator/(const Vec4& lhs, const Vec4& rhs)
    {
        return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
    }

    Vec4 operator/(const Vec4& lhs, float scalar)
    {
        const float div = 1.f / scalar;
        return { lhs.x * div, lhs.y * div, lhs.z * div, lhs.w * div };
    }

    bool operator==(const Vec4& lhs, const Vec4& rhs)
    {
        return FloatEqual(lhs.x, rhs.x) && FloatEqual(lhs.y, rhs.y) && FloatEqual(lhs.z, rhs.z) && FloatEqual(lhs.w, rhs.w);
    }

    bool operator!=(const Vec4& lhs, const Vec4& rhs)
    {
        return !(lhs == rhs);
    }
}
