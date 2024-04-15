#pragma once
#include "spark.h"

namespace se::shader
{
    namespace ast
    {
        class ShaderStage;
    }

    class ShaderGenerator
    {
    public:
        static std::optional<std::string> CompileShader(const std::string& filePath);
        static std::string AstToGlsl(const ast::ShaderStage& ast);
    };
}
