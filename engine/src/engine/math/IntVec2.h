#pragma once

#include "engine/reflect/Reflect.h"

namespace se::math
{
    struct Vec2;
    struct IntVec2
    {
        SPARK_POD_CLASS()

        IntVec2() : x(0), y(0)
        {
        }

        IntVec2(int _x, int _y) : x(_x), y(_y)
        {
        }

        IntVec2(const Vec2& vec);

        SPARK_MEMBER(Serialized)
        int x;

        SPARK_MEMBER(Serialized)
        int y;

        IntVec2& operator+=(const IntVec2 &rhs);
        IntVec2& operator+=(int scalar);
        IntVec2& operator-=(const IntVec2 &rhs);
        IntVec2& operator-=(int scalar);
        IntVec2& operator*=(const IntVec2 &rhs);
        IntVec2& operator*=(int scalar);
        IntVec2& operator*=(float scalar);
        IntVec2& operator/=(const IntVec2 &rhs);
        IntVec2& operator/=(int scalar);
        IntVec2& operator/=(float scalar);
    };

    IntVec2 operator-(const IntVec2 &lhs);
    IntVec2 operator+(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator+(const IntVec2& lhs, const Vec2 &rhs);
    IntVec2 operator-(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator-(const IntVec2& lhs, const Vec2 &rhs);
    IntVec2 operator*(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator*(const IntVec2& lhs, const Vec2 &rhs);
    IntVec2 operator*(const IntVec2& lhs, int scalar);
    IntVec2 operator*(const IntVec2& lhs, float scalar);
    IntVec2 operator/(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator/(const IntVec2& lhs, const Vec2 &rhs);
    IntVec2 operator/(const IntVec2& lhs, int scalar);
    IntVec2 operator/(const IntVec2& lhs, float scalar);
    bool operator==(const IntVec2& lhs, const IntVec2 &rhs);
    bool operator!=(const IntVec2& lhs, const Vec2 &rhs);
    bool operator<(const IntVec2 & lhs, const IntVec2& rhs);
    bool operator>(const IntVec2 & lhs, const IntVec2& rhs);
}

template <>
struct std::formatter<se::math::IntVec2> : std::formatter<std::string>
{
    template <class FormatContext>
    static auto format(const se::math::IntVec2& obj, FormatContext& ctx)
    {
        return std::format_to(ctx.out(), "IntVec2({}, {})", obj.x, obj.y);
    }
};
