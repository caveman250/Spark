#pragma once
#include "ASTNode.h"
#include "Types.h"

namespace se::asset::shader::ast
{
    class VariableDeclarationNode : public ASTNode
    {
        DECLARE_SPARK_CLASS(VariableDeclarationNode)

    public:
        VariableDeclarationNode() = default;
        VariableDeclarationNode(const std::string& name, AstType::Type type);
        std::string GetDebugString() const override;
        void ToGlsl(string::ArenaString& outShader) const override;

        void CollectUsedNames(std::map<std::string, std::string> &nameMap) const override;
        void ApplyNameRemapping(const std::map<std::string, std::string> &newNames) override;

        void SetName(const std::string& name) { m_Name = name; }

        AstType::Type GetType() { return m_Type; }
        const std::string& GetName() { return m_Name; }

    private:
        AstType::Type m_Type = {};
        std::string m_Name = {};
    };
}
