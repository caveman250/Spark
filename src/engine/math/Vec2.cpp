#include "Vec2.h"

#include "math.h"

namespace se::math
{
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
}
