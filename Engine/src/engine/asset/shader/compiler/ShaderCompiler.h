#pragma once
#include "spark.h"

namespace se::render
{
    class VertexBuffer;
}
namespace se::asset
{
    class Shader;
}
namespace se::asset::shader
{
    class ShaderCompiler
    {
    public:
        static std::optional<Shader> CompileShader(const std::string& filePath);
        static std::optional<std::string> GeneratePlatformShader(const std::vector<std::shared_ptr<Shader>>& shaderAssets, const render::VertexBuffer& vb);
        static std::string AstToGlsl(Shader& ast);
    };
}
