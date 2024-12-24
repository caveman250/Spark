#include "Vec2.h"

#include "IntVec2.h"
#include "math.h"

namespace se::math
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Vec2)
        DEFINE_SERIALIZED_MEMBER(x)
        DEFINE_SERIALIZED_MEMBER(y)
    DEFINE_SPARK_CLASS_END()

    Vec2::Vec2(const IntVec2 &v)
        : x(static_cast<float>(v.x))
        , y(static_cast<float>(v.y))
    {

    }


    Vec2& Vec2::operator+=(const Vec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2& Vec2::operator+=(float scalar)
    {
        x += scalar;
        y += scalar;
        return *this;
    }

    Vec2& Vec2::operator-=(const Vec2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vec2& Vec2::operator-=(float scalar)
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }

    Vec2& Vec2::operator*=(const Vec2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    Vec2& Vec2::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2& Vec2::operator/=(const Vec2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    Vec2& Vec2::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    float MagnitudeSquared(const Vec2 &vec)
    {
        return Dot(vec, vec);
    }

    Vec2 Normalized(const Vec2 &vec)
    {
        return vec * InverseSqrt(Dot(vec, vec));
    }

    float Dot(const Vec2 &v1, const Vec2 &v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    Vec2 operator-(const Vec2& lhs)
    {
        return {-lhs.x, -lhs.y };
    }

    Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
    {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
    {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    Vec2 operator*(const Vec2& lhs, const Vec2& rhs)
    {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }

    Vec2 operator*(const Vec2& lhs, float scalar)
    {
        return { lhs.x * scalar, lhs.y * scalar };
    }

    Vec2 operator/(const Vec2& lhs, const Vec2& rhs)
    {
        return { lhs.x / rhs.x, lhs.y / rhs.y };
    }

    Vec2 operator/(const Vec2& lhs, float scalar)
    {
        const float div = 1.f / scalar;
        return { lhs.x * div, lhs.y * div };
    }

    bool operator==(const Vec2& lhs, const Vec2& rhs)
    {
        return FloatEqual(lhs.x, rhs.x) && FloatEqual(lhs.y, rhs.y);
    }

    bool operator!=(const Vec2& lhs, const Vec2& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(const Vec2 &lhs, const Vec2 &rhs)
    {
        return MagnitudeSquared(lhs) < MagnitudeSquared(rhs);
    }

    bool operator>(const Vec2 &lhs, const Vec2 &rhs)
    {
        return MagnitudeSquared(lhs) > MagnitudeSquared(rhs);
    }
}
