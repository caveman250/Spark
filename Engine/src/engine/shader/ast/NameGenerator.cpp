#include "NameGenerator.h"

namespace se::shader::ast
{
    uint64_t NameGenerator::s_ID = {};

    std::string NameGenerator::GetName()
    {
        return std::format("temp{}", s_ID++);
    }
}
