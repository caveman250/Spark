#pragma once

#include "engine/reflect/Reflect.h"

namespace se::math
{
    struct Vec2
    {
        DECLARE_SPARK_CLASS(Vec2)

        Vec2() : x(0.f), y(0.f)
        {
        }

        Vec2(float _x, float _y) : x(_x), y(_y)
        {
        }

        union { float x, r, s; };
        union { float y, g, t; };

        Vec2& operator+=(const Vec2 &rhs);
        Vec2& operator+=(float scalar);
        Vec2& operator-=(const Vec2 &rhs);
        Vec2& operator-=(float scalar);
        Vec2& operator*=(const Vec2 &rhs);
        Vec2& operator*=(float scalar);
        Vec2& operator/=(const Vec2 &rhs);
        Vec2& operator/=(float scalar);
    };

    Vec2 operator-(const Vec2 &lhs);
    Vec2 operator+(const Vec2& lhs, const Vec2 &rhs);
    Vec2 operator-(const Vec2& lhs, const Vec2 &rhs);
    Vec2 operator*(const Vec2& lhs, const Vec2 &rhs);
    Vec2 operator*(const Vec2& lhs, float scalar);
    Vec2 operator/(const Vec2& lhs, const Vec2 &rhs);
    Vec2 operator/(const Vec2& lhs, float scalar);
    bool operator==(const Vec2& lhs, const Vec2 &rhs);
    bool operator!=(const Vec2& lhs, const Vec2 &rhs);
}

template <> struct std::formatter<se::math::Vec2>
{
    static constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static auto format(const se::math::Vec2& obj, std::format_context& ctx)
    {
        return std::format_to(ctx.out(), "vec2({}, {})", obj.x, obj.y);
    }
};
