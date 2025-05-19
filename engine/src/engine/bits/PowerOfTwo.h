#pragma once

#include "spark.h"

namespace se::bits
{
    template<typename T>
    concept Integer = std::is_integral_v<T>;

    template <Integer I>
    I RoundUpToPowerOf2(I x)
    {
        --x;
        if constexpr (sizeof(I) >= sizeof(uint64_t))
        {
            x |= (x >> 32);
        }
        if constexpr (sizeof(I) >= sizeof(uint32_t))
        {
            x |= (x >> 16);
        }
        if constexpr (sizeof(I) >= sizeof(uint16_t))
        {
            x |= (x >> 8);
        }
        x |= (x >>  4);
        x |= (x >>  2);
        x |= (x >>  1);
        return ++x;
    }
}
