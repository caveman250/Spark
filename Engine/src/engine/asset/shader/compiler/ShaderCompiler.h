#pragma once
#include "spark.h"
#include "engine/asset/shader/ShaderSettings.h"
#include "engine/render/TextureResource.h"

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
        static std::optional<std::string> GeneratePlatformShader(const std::vector<std::shared_ptr<Shader>>& shaderAssets,
                                                                    const ShaderSettings& settings,
                                                                    const render::VertexBuffer& vb,
                                                                    std::vector<std::pair<std::string, std::shared_ptr<render::TextureResource>>>& textureResources);

        static bool ResolveSettings(Shader& shader, const ShaderSettings& settings);
        static std::string AstToGlsl(Shader& ast, const ast::ShaderCompileContext& context);
        static std::string AstToMtl(Shader& ast, const ast::ShaderCompileContext& context);
    };
}
