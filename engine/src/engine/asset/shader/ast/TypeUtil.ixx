module;

#include "spark.h"

export module Spark.Asset.Shader:TypeUtil;
import :Types;

namespace se::asset::shader::ast
{
    export class TypeUtil
    {
    public:
        static std::string TypeToGlsl(AstType type);
        static AstType GlslToType(const std::string& type);
        static std::string TypeToMtl(AstType type);
        static AstType MtlToType(const std::string& type);
        static const std::vector<std::string>& GetTypeStrings();
        static size_t GetTypeSize(AstType type);
        static size_t GetTypePaddedSize(AstType type);
        static size_t GetTypeMetalAlignment(AstType type);
    };
}
