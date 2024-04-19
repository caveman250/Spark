#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    class ShaderStage;
}

namespace se::shader::ast
{
    class InputNode;
}

namespace se::shader::ast
{
    enum class Type;
}

namespace se::shader::ast
{
    class VariableReferenceNode : public ASTNode
    {
    public:
        VariableReferenceNode(const std::string& name, const ShaderStage& shaderStageAst);
        void ToGlsl(string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        void SetName(const std::string& name) { m_Name = name; }

        Type GetType() { return m_Type; }
        const std::string& GetName() { return m_Name; }

    private:
        Type m_Type;
        std::string m_Name;
    };
}
