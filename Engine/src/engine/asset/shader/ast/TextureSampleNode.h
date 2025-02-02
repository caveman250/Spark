#pragma once

#include "spark.h"
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class TextureSampleNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(TextureSampleNode)
    public:
        TextureSampleNode() {}
        TextureSampleNode(const std::string& samplerName, const std::string& uvVarName);
        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;
    private:
        std::string m_SamplerName = {};
        std::string m_UVVariableName = {};
    };
}
