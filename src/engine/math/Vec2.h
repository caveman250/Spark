#pragma once

namespace se::math
{
    struct Vec2
    {
        inline Vec2() : x(0.f), y(0.f)
        {
        }

        inline Vec2(float _x, float _y) : x(_x), y(_y)
        {
        }

        union
        {
            struct
            {
                float x;
                float y;
            };

            float asArray[2];
        };

        float operator[](int i) const
        {
            return asArray[i];
        }

        Vec2 operator+(const Vec2& rhs) const;
        Vec2 operator+=(const Vec2& rhs) const;
        Vec2 operator-(const Vec2& rhs) const;
        Vec2 operator*(const Vec2& rhs) const;
        Vec2 operator*(float scalar) const;
        Vec2 operator/(const Vec2& rhs) const;
        Vec2 operator/(float scalar) const;
        bool operator==(const Vec2& rhs) const;
        bool operator!=(const Vec2& rhs) const;
    };

    static Vec2 operator-(const Vec2& lhs) { return {-lhs.x, -lhs.y}; }
}

template <>
    struct std::formatter<se::math::Vec2>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const se::math::Vec2& obj, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "vec2({}, {})", obj.x, obj.y);
    }
};