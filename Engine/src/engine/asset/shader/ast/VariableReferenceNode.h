#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class Shader;
}

namespace se::asset::shader::ast
{
    class InputNode;
}

namespace se::asset::shader::ast
{
    enum class Type;
}

namespace se::asset::shader::ast
{
    class VariableReferenceNode : public ASTNode
    {
    public:
        VariableReferenceNode(const std::string& name, const Shader& shaderStageAst);
        std::string GetDebugString() const override;
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
