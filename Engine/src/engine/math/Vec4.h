#pragma once

#include "engine/reflect/Reflect.h"

namespace se::math
{
    struct Vec4
    {
        DECLARE_SPARK_POD_CLASS(Vec4)

        inline Vec4() : x(0.f), y(0.f), z(0.f), w(0.f)
        {
        }

        inline Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
        {
        }

        SPARK_MEMBER(Serialized)
        float x;

        SPARK_MEMBER(Serialized)
        float y;

        SPARK_MEMBER(Serialized)
        float z;

        SPARK_MEMBER(Serialized)
        float w;

        float& operator[](size_t i);
        const float& operator[](size_t i) const;

        Vec4& operator+=(const Vec4 &rhs);
        Vec4& operator+=(float scalar);
        Vec4& operator-=(const Vec4 &rhs);
        Vec4& operator-=(float scalar);
        Vec4& operator*=(const Vec4 &rhs);
        Vec4& operator*=(float scalar);
        Vec4& operator/=(const Vec4 &rhs);
        Vec4& operator/=(float scalar);
    };

    Vec4 operator-(const Vec4 &lhs);
    Vec4 operator+(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator-(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator*(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator*(const Vec4& lhs, float scalar);
    Vec4 operator/(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator/(const Vec4& lhs, float scalar);
    bool operator==(const Vec4& lhs, const Vec4 &rhs);
    bool operator!=(const Vec4& lhs, const Vec4 &rhs);
}

template <> struct std::formatter<se::math::Vec4>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const se::math::Vec4& obj, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "vec4({}, {}, {}, {})", obj.x, obj.y, obj.z, obj.w);
    }
};
