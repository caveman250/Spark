#pragma once

#include "spark.h"

namespace se::string::util
{
    bool Split(const String &string,
                     String &lhs,
                     String &rhs,
                     char deliminator,
                     bool fromEnd);
}
