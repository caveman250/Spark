#pragma once
#include "Types.h"

namespace se::asset::shader::ast
{
    class TypeUtil
    {
    public:
        static std::string TypeToGlsl(AstType::Type type);
        static AstType::Type GlslToType(const std::string& type);
        static std::string TypeToMtl(AstType::Type type);
        static AstType::Type MtlToType(const std::string& type);
        static const std::vector<std::string>& GetTypeStrings();
        static size_t GetTypeSize(AstType::Type type);
        static size_t GetTypePaddedSize(AstType::Type type);
    };
}
