#pragma once

#include "engine/reflect/Reflect.h"

namespace se::math
{
    struct Vec2;
    struct IntVec2
    {
        DECLARE_SPARK_POD_CLASS(IntVec2)

        IntVec2() : x(0), y(0)
        {
        }

        IntVec2(int _x, int _y) : x(_x), y(_y)
        {
        }

        IntVec2(const math::Vec2& vec);

        int x;
        int y;

        IntVec2& operator+=(const IntVec2 &rhs);
        IntVec2& operator+=(int scalar);
        IntVec2& operator-=(const IntVec2 &rhs);
        IntVec2& operator-=(int scalar);
        IntVec2& operator*=(const IntVec2 &rhs);
        IntVec2& operator*=(int scalar);
        IntVec2& operator/=(const IntVec2 &rhs);
        IntVec2& operator/=(int scalar);
    };

    IntVec2 operator-(const IntVec2 &lhs);
    IntVec2 operator+(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator-(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator*(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator*(const IntVec2& lhs, int scalar);
    IntVec2 operator/(const IntVec2& lhs, const IntVec2 &rhs);
    IntVec2 operator/(const IntVec2& lhs, int scalar);
    bool operator==(const IntVec2& lhs, const IntVec2 &rhs);
    bool operator!=(const IntVec2& lhs, const IntVec2 &rhs);
    bool operator<(const IntVec2 & lhs, const IntVec2& rhs);
    bool operator>(const IntVec2 & lhs, const IntVec2& rhs);
}

template <> struct std::formatter<se::math::IntVec2>
{
    static constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static auto format(const se::math::IntVec2& obj, std::format_context& ctx)
    {
        return std::format_to(ctx.out(), "IntVec2({}, {})", obj.x, obj.y);
    }
};
