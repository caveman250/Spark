#include "IntVec2.h"

#include "math.h"

namespace se::math
{
    IntVec2& IntVec2::operator+=(const IntVec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    IntVec2& IntVec2::operator+=(int scalar)
    {
        x += scalar;
        y += scalar;
        return *this;
    }

    IntVec2& IntVec2::operator-=(const IntVec2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    IntVec2& IntVec2::operator-=(int scalar)
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }

    IntVec2& IntVec2::operator*=(const IntVec2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    IntVec2& IntVec2::operator*=(int scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    IntVec2& IntVec2::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    IntVec2& IntVec2::operator/=(const IntVec2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    IntVec2& IntVec2::operator/=(int scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    IntVec2& IntVec2::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    IntVec2::IntVec2(const Vec2& vec)
        : x(static_cast<int>(vec.x))
        , y(static_cast<int>(vec.y))
    {
    }

    IntVec2 operator-(const IntVec2& lhs)
    {
        return { -lhs.x, -lhs.y };
    }

    IntVec2 operator+(const IntVec2& lhs,
                      const IntVec2& rhs)
    {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    IntVec2 operator+(const IntVec2& lhs,
                      const Vec2& rhs)
    {
        return { static_cast<int>(lhs.x + rhs.x), static_cast<int>(lhs.y + rhs.y) };
    }

    IntVec2 operator-(const IntVec2& lhs,
                      const IntVec2& rhs)
    {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    IntVec2 operator-(const IntVec2& lhs,
                      const Vec2& rhs)
    {
        return { static_cast<int>(lhs.x - rhs.x), static_cast<int>(lhs.y - rhs.y) };
    }

    IntVec2 operator*(const IntVec2& lhs,
                      const IntVec2& rhs)
    {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }

    IntVec2 operator*(const IntVec2& lhs,
                      const Vec2& rhs)
    {
        return { static_cast<int>(lhs.x * rhs.x), static_cast<int>(lhs.y * rhs.y) };
    }

    IntVec2 operator*(const IntVec2& lhs,
                      int scalar)
    {
        return { lhs.x * scalar, lhs.y * scalar };
    }

    IntVec2 operator*(const IntVec2& lhs,
                      float scalar)
    {
        return { static_cast<int>(lhs.x * scalar), static_cast<int>(lhs.y * scalar) };
    }

    IntVec2 operator/(const IntVec2& lhs,
                      const IntVec2& rhs)
    {
        return { lhs.x / rhs.x, lhs.y / rhs.y };
    }

    IntVec2 operator/(const IntVec2& lhs,
                      const Vec2& rhs)
    {
        return { static_cast<int>(lhs.x / rhs.x), static_cast<int>(lhs.y / rhs.y) };
    }

    IntVec2 operator/(const IntVec2& lhs,
                      int scalar)
    {
        return { lhs.x / scalar, lhs.y / scalar };
    }

    IntVec2 operator/(const IntVec2& lhs,
                      float scalar)
    {
        return { static_cast<int>(lhs.x / scalar), static_cast<int>(lhs.y / scalar) };
    }

    bool operator==(const IntVec2& lhs,
                    const IntVec2& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator!=(const IntVec2& lhs,
                    const IntVec2& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(const IntVec2& lhs,
                   const IntVec2& rhs)
    {
        return lhs.x + lhs.y < rhs.x + rhs.y;
    }

    bool operator>(const IntVec2& lhs,
                   const IntVec2& rhs)
    {
        return lhs.x + lhs.y > rhs.x + rhs.y;
    }
}
