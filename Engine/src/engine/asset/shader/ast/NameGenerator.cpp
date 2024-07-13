#include "NameGenerator.h"

namespace se::asset::shader::ast
{
    uint64_t NameGenerator::s_ID = {};

    std::string NameGenerator::GetName()
    {
        return std::format("temp{}", s_ID++);
    }
}
