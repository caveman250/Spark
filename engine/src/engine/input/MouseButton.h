#pragma once

namespace se::input
{
    SPARK_ENUM()
    enum class MouseButton
    {
        Unknown,
        Left,
        Right,
        Middle,
        Btn4,
        Btn5,
        None
    };

    inline std::underlying_type_t<MouseButton> operator | (const MouseButton& lhs, const MouseButton& rhs)
    {
        return static_cast<std::underlying_type_t<MouseButton>>(lhs) | static_cast<std::underlying_type_t<MouseButton>>(rhs);
    }

    inline std::underlying_type_t<MouseButton> operator & (const MouseButton& lhs, const MouseButton& rhs)
    {
        return static_cast<std::underlying_type_t<MouseButton>>(lhs) & static_cast<std::underlying_type_t<MouseButton>>(rhs);
    }

}
