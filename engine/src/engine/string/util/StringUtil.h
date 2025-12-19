#pragma once

#include "spark.h"

namespace se::string::util
{
    bool Split(const std::string &string,
                     std::string &lhs,
                     std::string &rhs,
                     char deliminator,
                     bool fromEnd);
}
