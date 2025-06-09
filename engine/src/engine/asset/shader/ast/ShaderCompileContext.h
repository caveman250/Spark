#pragma once

#include "engine/asset/shader/Shader.h"

namespace se::asset::shader::ast
{
    struct ShaderCompileContext
    {
        std::shared_ptr<Shader> vertShader = nullptr;
        std::shared_ptr<Shader> fragShader = nullptr;
        Shader* currentShader = nullptr;
        String vertexPositionOutputName = {}; // only used for Metal
        std::map<std::string, std::string> tempRenames ={}; // only used for Metal
        std::vector<std::pair<String, Variable>> fragmentShaderUniforms = {}; // only used for Metal
    };
}
