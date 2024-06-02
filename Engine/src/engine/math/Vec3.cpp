#include "Vec3.h"

#include "math.h"

namespace se::math
{
    DEFINE_SPARK_CLASS_BEGIN(Vec3)
        DEFINE_MEMBER(x)
        DEFINE_MEMBER(y)
        DEFINE_MEMBER(z)
    DEFINE_SPARK_CLASS_END()

    float& Vec3::operator[](size_t i)
    {
        SPARK_ASSERT(i >= 0 && i <= 2);
        switch(i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    const float& Vec3::operator[](size_t i) const
    {
        SPARK_ASSERT(i >= 0 && i <= 2);
        switch(i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

    Vec3& Vec3::operator+=(const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3& Vec3::operator+=(float scalar)
    {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    Vec3& Vec3::operator-=(const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vec3& Vec3::operator-=(float scalar)
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    Vec3& Vec3::operator*=(const Vec3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    Vec3& Vec3::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& Vec3::operator/=(const Vec3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    Vec3& Vec3::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    Vec3 operator-(const Vec3& lhs)
    {
        return {-lhs.x, -lhs.y, -lhs.z };
    }

    Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
    {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
    {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }

    Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
    {
        return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
    }

    Vec3 operator*(const Vec3& lhs, float scalar)
    {
        return { lhs.x * scalar, lhs.y * scalar, lhs.z * scalar };
    }

    Vec3 operator/(const Vec3& lhs, const Vec3& rhs)
    {
        return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
    }

    Vec3 operator/(const Vec3& lhs, float scalar)
    {
        const float div = 1.f / scalar;
        return { lhs.x * div, lhs.y * div, lhs.z * div };
    }

    bool operator==(const Vec3& lhs, const Vec3& rhs)
    {
        return FloatEqual(lhs.x, rhs.x) && FloatEqual(lhs.y, rhs.y) && FloatEqual(lhs.z, rhs.z);
    }

    bool operator!=(const Vec3& lhs, const Vec3& rhs)
    {
        return !(lhs == rhs);
    }

    Vec3 Normalized(const Vec3& vec)
    {
        return vec * InverseSqrt(Dot(vec, vec));
    }

    float Dot(const Vec3& v1, const Vec3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    Vec3 Cross(const Vec3& v1, const Vec3& v2)
    {
        return Vec3(v1.y * v2.z - v2.y * v1.z,
                    v1.z * v2.x - v2.z * v1.x,
                    v1.x * v2.y - v2.x * v1.y);
    }
}
