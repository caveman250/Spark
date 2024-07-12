#pragma once
#include "spark.h"

namespace se::render
{
    class VertexBuffer;
}

namespace se::shader
{
    namespace ast
    {
        class ShaderStage;
    }

    class ShaderGenerator
    {
    public:
        static std::optional<std::string> CompileShader(const std::vector<std::string>& filePaths, const render::VertexBuffer& vb);
        static std::string AstToGlsl(ast::ShaderStage& ast);
    };
}
