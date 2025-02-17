#pragma once

#include "engine/asset/shader/Shader.h"

namespace se::asset::shader::ast
{
    struct ShaderCompileContext
    {
        const Shader& shader;
        String vertexPositionOutputName = {}; // only used for Metal
        std::map<std::string, std::string> tempRenames;
    };
}
