#pragma once

#include "spark.h"
#include "ASTNode.h"

namespace se::shader::ast
{
    class TextureSampleNode : public ASTNode
    {
    public:
        TextureSampleNode(const std::string& samplerName, const std::string& uvVarName);
        void ToGlsl(string::ArenaString& outShader) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;
    private:
        std::string m_SamplerName;
        std::string m_UVVariableName;
    };
}
