#include "IntVec2.h"

#include "math.h"

namespace se::math
{
    DEFINE_SPARK_POD_CLASS_BEGIN(IntVec2)
        DEFINE_MEMBER(x)
        DEFINE_MEMBER(y)
    DEFINE_SPARK_CLASS_END()

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

    IntVec2 operator-(const IntVec2& lhs)
    {
        return {-lhs.x, -lhs.y };
    }

    IntVec2 operator+(const IntVec2& lhs, const IntVec2& rhs)
    {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    IntVec2 operator-(const IntVec2& lhs, const IntVec2& rhs)
    {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    IntVec2 operator*(const IntVec2& lhs, const IntVec2& rhs)
    {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }

    IntVec2 operator*(const IntVec2& lhs, int scalar)
    {
        return { lhs.x * scalar, lhs.y * scalar };
    }

    IntVec2 operator/(const IntVec2& lhs, const IntVec2& rhs)
    {
        return { lhs.x / rhs.x, lhs.y / rhs.y };
    }

    IntVec2 operator/(const IntVec2& lhs, int scalar)
    {
        return { lhs.x / scalar, lhs.y / scalar };
    }

    bool operator==(const IntVec2& lhs, const IntVec2& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator!=(const IntVec2& lhs, const IntVec2& rhs)
    {
        return !(lhs == rhs);
    }

    bool operator<(const IntVec2 &lhs, const IntVec2 &rhs)
    {
        return lhs.x + lhs.y < rhs.x + rhs.y;
    }

    bool operator>(const IntVec2 &lhs, const IntVec2 &rhs)
    {
        return lhs.x + lhs.y > rhs.x + rhs.y;
    }
}
