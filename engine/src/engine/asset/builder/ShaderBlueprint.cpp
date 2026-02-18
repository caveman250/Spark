#include "ShaderBlueprint.h"
#include "spark.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/reflect/Util.h"

namespace se::asset::builder
{
    std::regex ShaderBlueprint::GetFilePattern() const
    {
        return std::regex(".*.ssl");
    }

    std::vector<BuiltAsset> ShaderBlueprint::BuildAsset(const std::string& path, const std::string&) const
    {
        auto shader = shader::ShaderCompiler::CompileShader(path);
        shader->m_SourcePath = path;
        if (shader.has_value())
        {
            return { { reflect::SerialiseType<Shader>(&shader.value()), "" } };
        }

        return { };
    }
}
