#pragma once
#include "spark.h"
#include "engine/asset/shader/ShaderSettings.h"

namespace se
{
    class ShaderSettings;
}

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
        static std::optional<std::string> GeneratePlatformShader(const std::vector<std::shared_ptr<Shader>>& shaderAssets, const ShaderSettings& settings,  const render::VertexBuffer& vb);

        static bool ResolveSettings(Shader& shader, const ShaderSettings& settings);
        static std::string AstToGlsl(Shader& ast);
    };
}
