#pragma once

#include "spark.h"
#include "Type.h"

namespace se::asset::binary
{
    typedef std::array<char, 32> FixedString32;
    typedef std::vector<std::pair<FixedString32, Type>> StructLayout;
    FixedString32 CreateFixedString32(const char* c);
}