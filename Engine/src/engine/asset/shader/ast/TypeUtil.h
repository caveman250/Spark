#pragma once
#include "Types.h"

namespace se::asset::shader::ast
{
    class TypeUtil
    {
    public:
        static std::string GetTypeGlsl(AstType::Type type);
        static AstType::Type StringToType(const std::string& type);
        static const std::vector<std::string>& GetTypeStrings();
    };
}
