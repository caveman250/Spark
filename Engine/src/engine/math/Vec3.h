#pragma once

#include "engine/reflect/Reflect.h"

namespace se::math
{
    struct Vec3
    {
        DECLARE_SPARK_POD_CLASS(Vec3)

        Vec3() : x(0.f), y(0.f), z(0.f)
        {
        }

        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
        {
        }

        float x;
        float y;
        float z;

        float& operator[](size_t i);
        const float& operator[](size_t i) const;

        Vec3& operator+=(const Vec3& rhs);
        Vec3& operator+=(float scalar);
        Vec3& operator-=(const Vec3& rhs);
        Vec3& operator-=(float scalar);
        Vec3& operator*=(const Vec3& rhs);
        Vec3& operator*=(float scalar);
        Vec3& operator/=(const Vec3& rhs);
        Vec3& operator/=(float scalar);
    };

    Vec3 operator-(const Vec3& lhs);
    Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
    Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
    Vec3 operator*(const Vec3& lhs, const Vec3& rhs);
    Vec3 operator*(const Vec3& lhs, float scalar);
    Vec3 operator/(const Vec3& lhs, const Vec3& rhs);
    Vec3 operator/(const Vec3& lhs, float scalar);
    bool operator==(const Vec3& lhs, const Vec3& rhs);
    bool operator!=(const Vec3& lhs, const Vec3& rhs);

    float MagnitudeSquared(const Vec3& vec);
    Vec3 Normalized(const Vec3& vec);
    float Dot(const Vec3& v1, const Vec3& v2);
    Vec3 Cross(const Vec3& v1, const Vec3& v2);
}

template <>
struct std::formatter<se::math::Vec3>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <class FormatContext>
    auto format(const se::math::Vec3& obj, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "vec3({}, {}, {})", obj.x, obj.y, obj.z);
    }
};
