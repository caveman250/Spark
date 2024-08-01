#include "spark.h"
#include "ShaderBlueprint.h"
#include "engine/asset/shader/Shader.h"
#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/reflect/Util.h"

namespace se::asset::builder
{
    std::regex ShaderBlueprint::GetFilePattern() const
    {
        return std::regex(".*.ssl");
    }

    std::shared_ptr<binary::Database> ShaderBlueprint::BuildAsset(const std::string& path, asset::meta::MetaData&) const
    {
        auto shader = shader::ShaderCompiler::CompileShader(path);
        if (shader.has_value())
        {
            return reflect::SerialiseType<Shader>(&shader.value());
        }

        return nullptr;
    }
}
