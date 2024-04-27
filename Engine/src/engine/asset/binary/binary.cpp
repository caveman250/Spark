#include "binary.h"

namespace se::asset::binary
{
    FixedString32 CreateFixedString32(const char* c)
    {
        SPARK_ASSERT(strlen(c) <= 32);

        FixedString32 newStr {0};
        std::memcpy(newStr.data(), c, strlen(c));

        return newStr;
    }
}