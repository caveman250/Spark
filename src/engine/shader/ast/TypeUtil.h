#pragma once
#include "Types.h"

namespace se::shader::ast
{
    class TypeUtil
    {
    public:
        static std::string GetTypeGlsl(Type type);
        static Type StringToType(const std::string& type);
    };
}
