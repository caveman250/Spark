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
        static std::optional<ast::Shader> CompileShader(const std::string& filePath);
        static std::optional<std::string> GeneratePlatformShader(const std::vector<std::shared_ptr<ast::Shader>>& shaderAssets, const render::VertexBuffer& vb);
        static std::string AstToGlsl(ast::Shader& ast);
    };
}
