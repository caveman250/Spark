#pragma once

#include "spark.h"

namespace se::string::util
{
    int constexpr ConstLength(const char* str)
    {
        return *str ? 1 + ConstLength(str + 1) : 0;
    }

    bool Split(const std::string &string,
                     std::string &lhs,
                     std::string &rhs,
                     char deliminator,
                     bool fromEnd);
}
