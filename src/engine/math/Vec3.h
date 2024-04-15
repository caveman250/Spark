#pragma once

namespace se::math
{
    struct Vec3
    {
        inline Vec3() : x(0.f), y(0.f), z(0.f)
        {
        }

        inline Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
        {
        }

        union
        {
            struct
            {
                float x;
                float y;
                float z;
            };

            float asArray[3];
        };

        float operator[](int i) const
        {
            return asArray[i];
        }

        Vec3 operator+(const Vec3& rhs) const;

        Vec3 operator+=(const Vec3& rhs) const;

        Vec3 operator-(const Vec3& rhs) const;

        Vec3 operator*(const Vec3& rhs) const;

        Vec3 operator*(float scalar) const;

        Vec3 operator/(const Vec3& rhs) const;

        Vec3 operator/(float scalar) const;

        bool operator==(const Vec3& rhs) const;

        bool operator!=(const Vec3& rhs) const;
    };

    static Vec3 operator-(const Vec3& lhs) { return {-lhs.x, -lhs.y, -lhs.z}; }
}

template <>
    struct std::formatter<se::math::Vec3>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const se::math::Vec3& obj, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "vec3({}, {}, {})", obj.x, obj.y, obj.z);
    }
};
