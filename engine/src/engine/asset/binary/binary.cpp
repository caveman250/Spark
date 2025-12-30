#include "binary.h"

namespace se::asset::binary
{
    FixedString64 CreateFixedString64(const char* c)
    {
        SPARK_ASSERT(strlen(c) <= 64);

        FixedString64 newStr {0};
        std::memcpy(newStr.data(), c, strlen(c));

        return newStr;
    }
}