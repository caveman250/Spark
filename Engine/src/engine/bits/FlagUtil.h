#pragma once

namespace se::bits
{
    template<typename T> concept Enum = std::is_enum_v<T>;

    template <Enum E>
    void SetFlag(int32_t& src, E flag)
    {
        int32_t mask = 1 << static_cast<int32_t>(flag);
        src |= mask;
    }

    template <Enum E>
    bool GetFlag(int32_t src, E flag)
    {
        int32_t mask = 1 << static_cast<int32_t>(flag);
        return src & mask;
    }


    template <Enum E>
    void UnsetFlag(int32_t& src, E flag)
    {
        int32_t mask = 1 << static_cast<int32_t>(flag);
        src &= ~mask;
    }
}