#pragma once
#include "ASTNode.h"
#include "Types.h"
#include "Variable.h"

namespace se::asset::shader::ast
{
    class VariableDeclarationNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(VariableDeclarationNode)

    public:
        VariableDeclarationNode() = default;
        VariableDeclarationNode(const std::string& name, const Variable& var);
        std::string GetDebugString() const override;
        void ToGlsl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        void SetName(const std::string& name) { m_Name = name; }

        const Variable& GetVar() { return m_Var; }
        const std::string& GetName() { return m_Name; }

    private:
        Variable m_Var = {};
        std::string m_Name = {};
    };
}
