#pragma once
#include "ASTNode.h"
#include "Operators.h"

namespace se::shader::ast
{
    class OperatorNode : public ASTNode
    {
    public:
        OperatorNode(OperatorType type);
        void ToGlsl(string::ArenaString& outShader) const override;
    private:
        OperatorType m_Type;
    };
}
