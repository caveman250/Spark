#pragma once

#include "spark.h"

namespace se::bits
{
    inline int64_t Pack64(int32_t a, int32_t b)
    {
        return static_cast<int64_t>((static_cast<uint64_t>(a) << 32) + static_cast<uint64_t>(b));
    }

    inline int32_t UnpackA64(int64_t x)
    {
        return static_cast<int32_t>(static_cast<uint64_t>(x) >> 32);
    }

    inline int32_t UnpackB64(int64_t x)
    {
        return static_cast<int32_t>(static_cast<uint64_t>(x) & 0xffffffff);
    }
}
