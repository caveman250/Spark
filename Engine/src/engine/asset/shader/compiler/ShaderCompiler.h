#pragma once
#include "spark.h"

namespace se::render
{
    class VertexBuffer;
}

namespace se::asset::shader
{
    namespace ast
    {
        class Shader;
    }

    class ShaderCompiler
    {
    public:
        static std::optional<std::string> CompileShader(const std::vector<std::string>& filePaths, const render::VertexBuffer& vb);
        static std::string AstToGlsl(ast::Shader& ast);
    };
}
