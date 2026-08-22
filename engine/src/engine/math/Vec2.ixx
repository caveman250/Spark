module;

#include "spark.h"

export module Spark.Math:Vec2;

import Reflect;

namespace se::math
{
    struct IntVec2;
    export struct Vec2
    {
        SPARK_POD_CLASS()

        Vec2() : x(0.f), y(0.f)
        {
        }

        Vec2(float scalar) : x(scalar), y(scalar)
        {
        }

        Vec2(float _x, float _y) : x(_x), y(_y)
        {
        }

        Vec2(const IntVec2 &v);

        SPARK_MEMBER(Serialized)
        float x;

        SPARK_MEMBER(Serialized)
        float y;

        Vec2& operator+=(const Vec2 &rhs);
        Vec2& operator+=(float scalar);
        Vec2& operator-=(const Vec2 &rhs);
        Vec2& operator-=(float scalar);
        Vec2& operator*=(const Vec2 &rhs);
        Vec2& operator*=(float scalar);
        Vec2& operator/=(const Vec2 &rhs);
        Vec2& operator/=(float scalar);
    };

    export float MagnitudeSquared(const Vec2& vec);
    export Vec2 Normalized(const Vec2& vec);
    export float Dot(const Vec2& v1, const Vec2& v2);
    export Vec2 Min(const Vec2& a, const Vec2& b);
    export Vec2 Max(const Vec2& a, const Vec2& b);

    export Vec2 operator-(const Vec2 &lhs);
    export Vec2 operator+(const Vec2& lhs, const Vec2 &rhs);
    export Vec2 operator-(const Vec2& lhs, const Vec2 &rhs);
    export Vec2 operator*(const Vec2& lhs, const Vec2 &rhs);
    export Vec2 operator*(const Vec2& lhs, float scalar);
    export Vec2 operator/(const Vec2& lhs, const Vec2 &rhs);
    export Vec2 operator/(const Vec2& lhs, float scalar);
    export bool operator==(const Vec2& lhs, const Vec2 &rhs);
    export bool operator!=(const Vec2& lhs, const Vec2 &rhs);
    export bool operator<(const Vec2 & lhs, const Vec2& rhs);
    export bool operator>(const Vec2 & lhs, const Vec2& rhs);
}

export template <>
struct std::formatter<se::math::Vec2>
{
    static constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <class FormatContext>
    static auto format(const se::math::Vec2& obj, FormatContext& ctx)
    {
        return std::format_to(ctx.out(), "vec2({}, {})", obj.x, obj.y);
    }
};
