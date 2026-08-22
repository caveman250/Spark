module;

#include <spark.h>
#include <map>

export module Spark.Asset.Shader:ShaderCompileContext;

namespace se::asset
{
    class Shader;
}

namespace se::asset::shader::ast
{
    struct Variable;
    export struct ShaderCompileContext
    {
        std::shared_ptr<Shader> vertShader = nullptr;
        std::shared_ptr<Shader> fragShader = nullptr;
        Shader* currentShader = nullptr;
        std::string vertexPositionOutputName = {}; // only used for Metal
        std::map<std::string, std::string> tempRenames ={}; // only used for Metal
        std::vector<std::pair<std::string, Variable>> fragmentShaderUniforms = {}; // only used for Metal
    };
}
