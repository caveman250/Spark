#pragma once

#include <array>
#include <vector>
#include "Type.h"

namespace se::asset::binary
{
    typedef std::array<char, 64> FixedString64;
    typedef std::vector<std::pair<FixedString64, Type>> StructLayout;
    FixedString64 CreateFixedString64(const char* c);
}