#pragma once

#include "spark.h"
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class TextureSampleNode : public ASTNode
    {
        SPARK_CLASS()
    public:
        TextureSampleNode() {}
        TextureSampleNode(const std::string& samplerName, const std::string& uvVarName);
        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtlPreDeclarations(ShaderCompileContext& context, string::ArenaString& outShader) override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;
    private:
        SPARK_MEMBER(Serialized)
        std::string m_SamplerName = {};

        SPARK_MEMBER(Serialized)
        std::string m_UVVariableName = {};

        // only used during shader generation
        String m_TempSamplerName = {};
    };
}
