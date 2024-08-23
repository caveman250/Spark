#pragma once

#include "spark.h"

namespace se::bits
{
    inline uint32_t RoundUpToPowerOf2(uint32_t x)
    {
        --x;
        x |= (x >> 16);
        x |= (x >>  8);
        x |= (x >>  4);
        x |= (x >>  2);
        x |= (x >>  1);
        return ++x;
    }

    inline uint64_t RoundUpToPowerOf2(uint64_t x)
    {
        --x;
        x |= (x >> 32);
        x |= (x >> 16);
        x |= (x >>  8);
        x |= (x >>  4);
        x |= (x >>  2);
        x |= (x >>  1);
        return ++x;
    }
}
