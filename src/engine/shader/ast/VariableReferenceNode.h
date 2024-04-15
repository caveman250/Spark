#pragma once
#include "ASTNode.h"

namespace se::shader::ast
{
    enum class Type;
}

namespace se::shader::ast
{
    class VariableNode : public ASTNode
    {
    public:
        VariableNode(Type type);
        std::string ToGlsl() const override;

        Type GetType() { return m_Type; }
        const std::string& GetName() { return m_Name; }

    private:
        Type m_Type;
        std::string m_Name;
    };
}
